//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "RFCommServer.h"

#include <btstack.h>
#include <string.h>

static const int RFCOMM_SERVER_CHANNEL = 1;

namespace bluepadhub {

    void RFCommServer::begin() {

        hci_event_callback_registration.callback = &packetHandler;
        hci_add_event_handler(&hci_event_callback_registration);

        rfcomm_init();
        rfcomm_register_service(packetHandler, RFCOMM_SERVER_CHANNEL, 0xffff); 

        memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
        spp_create_sdp_record(spp_service_buffer, sdp_create_service_record_handle(), RFCOMM_SERVER_CHANNEL, "RFCOMM");

        // 0x000C "IconURL"
        //de_add_number(spp_service_buffer, DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_ICON_URL ); 
        //de_add_number(spp_service_buffer, DE_UINT, DE_SIZE_16, 0x04C0 );

        btstack_assert(de_get_len( spp_service_buffer) <= sizeof(spp_service_buffer));
        sdp_register_service(spp_service_buffer);

        gap_ssp_set_io_capability(SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT);   
        //gap_set_link_supervision_timeout(0);
    }

    void RFCommServer::setDeviceName(const char* deviceName) {

        if (strlen(deviceName) < sizeof(this->deviceName)) {
            strcpy(this->deviceName, deviceName);
            gap_set_local_name(this->deviceName); 
        }        
    }

    void RFCommServer::setDiscoverable(bool discoverable) {

        if (discoverable)
            gap_discoverable_control(1);  
        else
            gap_discoverable_control(0);  
    }

    void RFCommServer::setEventListener(RFCommServer::Listener *_listener) {

        listener = _listener;
    }

    void RFCommServer::packetHandler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {

        bd_addr_t event_addr;
        uint8_t   rfcomm_channel_nr;
        uint8_t   rfcomm_channel_id;
        uint16_t  mtu;
        int i;
        RFCommStream* s;

        switch (packet_type) {
            case HCI_EVENT_PACKET:
                switch (hci_event_packet_get_type(packet)) {

                    // these events are handled in Bluepad32's handler
                    /*
                    case HCI_EVENT_PIN_CODE_REQUEST:
                        // inform about pin code request
                        printf("Pin code request - using '0000'\n");
                        hci_event_pin_code_request_get_bd_addr(packet, event_addr);
                        gap_pin_code_response(event_addr, "0000");
                        break;                    
                    
                    case HCI_EVENT_USER_CONFIRMATION_REQUEST:
                        // ssp: inform about user confirmation request
                        printf("SSP User Confirmation Request with numeric value '%06"PRIu32"'\n", little_endian_read_32(packet, 8));
                        printf("SSP User Confirmation Auto accept\n");
                        break;
                    */

                    case RFCOMM_EVENT_INCOMING_CONNECTION:
                        rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr);
                        rfcomm_channel_nr = rfcomm_event_incoming_connection_get_server_channel(packet);
                        rfcomm_channel_id = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
                        printf("RFCOMM channel %u requested for %s\n", rfcomm_channel_nr, bd_addr_to_str(event_addr));
                        rfcomm_accept_connection(rfcomm_channel_id);
                        break;
                
                    case RFCOMM_EVENT_CHANNEL_OPENED:
                        if (rfcomm_event_channel_opened_get_status(packet)) {
                            printf("RFCOMM channel open failed, status 0x%02x\n", rfcomm_event_channel_opened_get_status(packet));
                        } else {
                            rfcomm_channel_id = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
                            mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
                            s = new RFCommStream(rfcomm_channel_id, mtu);
                            streams[rfcomm_channel_id] = s;

                            if (listener != nullptr)
                                listener->streamOpened(s);

                            printf("RFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u\n", rfcomm_channel_id, mtu);
                        }
                        break;

                    case RFCOMM_EVENT_CHANNEL_CLOSED:
                        rfcomm_channel_id = rfcomm_event_channel_closed_get_rfcomm_cid(packet);
                        s = streams.at(rfcomm_channel_id);
                        if (s == nullptr) {
                            printf("RFCOMM stream not found\n");    
                        } else {
                            streams.erase(rfcomm_channel_id);
                            s->close();

                            if (listener != nullptr)
                                listener->streamClosed(s);

                            delete s;
                        }
                        printf("RFCOMM channel closed\n");
                        break;

                    case RFCOMM_EVENT_CAN_SEND_NOW:
                        rfcomm_channel_id = rfcomm_event_can_send_now_get_rfcomm_cid(packet);                        
                        s = streams.at(rfcomm_channel_id);
                        if (s == nullptr) {
                            printf("RFCOMM stream not found\n");    
                        } else {
                            s->sendDataPacket();
                        }
                        break;
                    
                    default:
                        break;
                }
                break;

            case RFCOMM_DATA_PACKET:
                rfcomm_channel_id = channel;
                s = streams.at(rfcomm_channel_id);
                if (s == nullptr) {
                    printf("RFCOMM stream not found\n");    
                } else {
                    s->receiveDataPacket(packet, size);
                }
                break;

            default:
                break;
        }

    }

}
