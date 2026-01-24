//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _RFCOMM_STREAM_H_
#define _RFCOMM_STREAM_H_

#include <Arduino.h>
#include <btstack.h>
#include <string>
#include "freertos/semphr.h"

namespace bluepadhub {

    class RFCommStream : public Stream {
        protected:
            
            friend class RFCommServer;

            RFCommStream(int rfcommChannel, int mtu) {
                this->rfcommChannel = rfcommChannel;
                this->mtu = mtu;

                btstack_send_buffer = new uint8_t[mtu];

                mutex_receive = xSemaphoreCreateMutex();
                mutex_send = xSemaphoreCreateMutex();

                readBuffer.reserve(2048);
                receiveBuffer.reserve(2048);
                sendBuffer.reserve(2048);
            };

            void receiveDataPacket(uint8_t *packet, uint16_t size);
            void sendDataPacket();

            void close() {
                closed = true;
            };

        public:
            virtual ~RFCommStream() {
                delete [] btstack_send_buffer;
            };

            bool isClosed() {
                return closed;
            };

            virtual size_t write(uint8_t b);
            virtual size_t write(const uint8_t *buffer, size_t size);
            virtual int available();
            virtual int read();
            virtual int peek();
            virtual size_t readBytes(char *buffer, size_t length);
            virtual int availableForWrite();

        private:

            std::string receiveBuffer = "";
            std::string readBuffer = "";
            std::string sendBuffer = "";

            int rfcommChannel = 0;
            int mtu = 0;
            bool closed = false;

            uint8_t *btstack_send_buffer;     
            btstack_context_callback_registration_t btstack_callback_registration;


            SemaphoreHandle_t mutex_receive = nullptr;
            SemaphoreHandle_t mutex_send = nullptr;
    };
}

#endif