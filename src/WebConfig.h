//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _WEB_CONFIG_H_
#define _WEB_CONFIG_H_

#include <ArduinoJson.h>
#include "RFCommServer.h"

#include <functional>

namespace bluepadhub {

    class StreamPointerWriter {

        public:
            StreamPointerWriter(Stream* stream) {
                this->stream = stream;
            };

            size_t write(uint8_t c) {
                return stream->write(c);
            };

            size_t write(const uint8_t *buffer, size_t length) {
                return stream->write(buffer, length);
            };
        
        private:
            Stream *stream;
    };    

    class WebConfig : private RFCommServer::Listener {
        public:

            WebConfig() {}

            void begin();
            void setDeviceName(std::string deviceName);
            void setDiscoverable(int seconds);

            virtual void loadDefaultSettings() {};
            virtual void applySettings() {};

            virtual bool loadSettingsFromJson(JsonDocument &json) { return false; };
            virtual JsonDocument saveSettingsToJson() { return JsonDocument(); }; 

        private:

            struct StreamListenerTaskParams {
                WebConfig *obj;
                Stream *stream;
                std::function<bool()> checkStreamClosed;
            };

            void loadSettingsFromStorage();
            void saveSettingsToStorage();

            JsonDocument processRequest(JsonDocument &request);

            void streamOpened(bluepadhub::RFCommStream *stream);            

            void startStreamListener(Stream* stream, std::function<bool()> checkStreamClosed);
            void streamListenerTask(StreamListenerTaskParams *taskParams);

            RFCommServer rfcomm;
            unsigned long discoverableTimeoutMillis;
    };

}

#endif