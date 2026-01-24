//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "RFCommStream.h"

namespace bluepadhub {
        
    size_t RFCommStream::write(uint8_t b){

        return write(&b, sizeof(b));
    }

    size_t RFCommStream::write(const uint8_t *buffer, size_t size) {

        xSemaphoreTake(mutex_send, portMAX_DELAY);
            sendBuffer.append(reinterpret_cast<const char*>(buffer), size);
        xSemaphoreGive(mutex_send);        
        

        btstack_callback_registration.callback = [](void* ctx) {
            rfcomm_request_can_send_now_event(static_cast<RFCommStream*>(ctx)->rfcommChannel);
        };
        btstack_callback_registration.context = this;
        btstack_run_loop_execute_on_main_thread(&btstack_callback_registration);  

        return size;
    }

    int RFCommStream::available() {

        xSemaphoreTake(mutex_receive, portMAX_DELAY);
            readBuffer.append(receiveBuffer);
            receiveBuffer.clear();
        xSemaphoreGive(mutex_receive);

        return readBuffer.length();
    }

    int RFCommStream::read() {

        char b;
        if (readBytes(&b, sizeof(b)) > 0)
            return b;

        return -1;
    }

    int RFCommStream::peek() {

        if (available() > 0)
            return readBuffer.at(0);

        return -1;
    }

    size_t RFCommStream::readBytes(char *buffer, size_t length) {
        
        unsigned long timeout = millis() + getTimeout();

        // wait until read buffer contains data of requested size or timeout occurs
        while (available() < length) {

            vTaskDelay(1);

            if (millis() > timeout)
                break;
        }

        size_t read_len = min((int)available(), (int)length);

        if (read_len > 0) {
            
            readBuffer.copy(buffer, read_len);
            readBuffer.erase(0, read_len);

            return read_len;
        }

        return 0;
    }

    int RFCommStream::availableForWrite() {

        xSemaphoreTake(mutex_send, portMAX_DELAY);
            int len = sendBuffer.length();
        xSemaphoreGive(mutex_send);

        return 2048 - len;
    }

    void RFCommStream::receiveDataPacket(uint8_t *packet, uint16_t size) {

        xSemaphoreTake(mutex_receive, portMAX_DELAY);
            receiveBuffer.append(reinterpret_cast<const char*>(packet), size);
        xSemaphoreGive(mutex_receive);
    }

    void RFCommStream::sendDataPacket() {

        bool send_next = false;

        xSemaphoreTake(mutex_send, portMAX_DELAY);

            int send_size = sendBuffer.length();

            if (send_size > mtu) {
                send_size = mtu;
                send_next = true;
            }

            if (send_size > 0) {
                sendBuffer.copy(reinterpret_cast<char*>(btstack_send_buffer), send_size);
                sendBuffer.erase(0, send_size);
            }

        xSemaphoreGive(mutex_send);

        if (send_size > 0)
            rfcomm_send(rfcommChannel, btstack_send_buffer, send_size);  

        if (send_next)
            rfcomm_request_can_send_now_event(rfcommChannel);

    }
}