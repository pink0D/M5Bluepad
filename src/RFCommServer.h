//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _RFCOMM_SERVER_H_
#define _RFCOMM_SERVER_H_

#include <Arduino.h>
#include <string>
#include <map>

#include "RFCommStream.h"

namespace bluepadhub {


    class RFCommServer {
        public:

            class Listener {
                public:
                    Listener() {};

                    virtual void streamOpened(RFCommStream *stream) {};
                    virtual void streamClosed(RFCommStream *stream) {};
            };

            RFCommServer() {};

            void begin();
            void setDeviceName(const char* deviceName);
            void setDiscoverable(bool discoverable);
            void setEventListener(Listener *_listener);

        private:
            static void packetHandler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

            static inline Listener *listener = nullptr;
            static inline std::map<int, RFCommStream*> streams;

            btstack_packet_callback_registration_t hci_event_callback_registration;
            uint8_t spp_service_buffer[150];
            char deviceName[32];


    };
}

#endif