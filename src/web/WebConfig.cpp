//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "WebConfig.h"
#include "BluepadHub.h"

#include <FS.h>
#include <SPIFFS.h>

static const char* configFileName = "/WebConfig.json";
static const std::string strJsonStart = "====JSON START====";
static const std::string strJsonEnd = "====JSON END====";


namespace bluepadhub {

    void WebConfig::begin() {

        loadDefaultSettings();
        loadSettingsFromStorage();
        applySettings();


        // start Bluetooth Serial Port
        rfcomm.setEventListener(this);
        rfcomm.begin();        

        // start default listener for Serial port
        startStreamListener(&Serial, nullptr);
    }

    void WebConfig::loadSettingsFromStorage() {

        if (!SPIFFS.begin(true)) {
            Serial.println("WARNING: SPIFFS begin Failed");
            return;
        }

        File file = SPIFFS.open(configFileName, FILE_READ);
        if (!file) {
            Serial.println("WARNING: Failed to open config file for reading");
            return;
        }

        JsonDocument json; 

        DeserializationError error = deserializeJson(json, file);

        file.close();

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        loadSettingsFromJson(json);
    }

    void WebConfig::saveSettingsToStorage() {

        File file = SPIFFS.open(configFileName, FILE_WRITE);
        if (!file) {
            Serial.println("WARNING: Failed to open config file for writing");
            return;
        }

        if (serializeJson(saveSettingsToJson(), file) == 0) {
            Serial.println("Failed to write to config file");
        }

        file.close();
    }    

    void WebConfig::setDeviceName(std::string deviceName) {
        rfcomm.setDeviceName(deviceName.c_str());
    }


    void WebConfig::setDiscoverable(int seconds) {

        discoverableTimeoutMillis = seconds * 1000;
        
        xTaskCreate( [](void* ctx) {

            WebConfig *obj = static_cast<WebConfig*>(ctx);

            obj->rfcomm.setDiscoverable(true);
            vTaskDelayMillis(obj->discoverableTimeoutMillis);
            obj->rfcomm.setDiscoverable(false);
            vTaskDelete(NULL);

        }, "RfCommDiscoverable", 2*1024, this, 0, nullptr);

        ::BluepadHub.getStatusIndicator()->setEventPattern(bluepadhub::StatusIndicator::EventPattern::Info);
    }

    void WebConfig::streamOpened(bluepadhub::RFCommStream *stream) {

        startStreamListener(stream, [stream]() -> bool { return stream->isClosed(); } );
    };

    void WebConfig::startStreamListener(Stream* stream, std::function<bool()> checkStreamClosed) {

        StreamListenerTaskParams *params = new StreamListenerTaskParams;
        params->obj = this;
        params->stream = stream;
        params->checkStreamClosed = checkStreamClosed;

        xTaskCreate( [](void* ctx) {

            StreamListenerTaskParams *params = static_cast<StreamListenerTaskParams*>(ctx);
            params->obj->streamListenerTask(params);

        }, "streamListenerTask", TASK_STACK_SIZE, params, 0, NULL);
    }

    void WebConfig::streamListenerTask(StreamListenerTaskParams *taskParams) {

        Stream *stream = taskParams->stream;
        std::string input = "";

        Serial.println("Starting streamListenerTask");

        while(1) {

            char recv_buffer[256];
            int read_count = 0;

            // read all data from stream
            while ( (read_count = min(stream->available(), (int)sizeof(recv_buffer)) ) > 0) {
                
                // append new data to input buffer
                stream->readBytes(recv_buffer, read_count);
                input += std::string(recv_buffer, read_count);
                //Serial.printf("read count = %d \r\n", read_count);
                //Serial.println("current buffer");
                //Serial.println(input.c_str());

                // process all fully received JSONs in input buffer
                int jsonEnd;
                while ( (jsonEnd = input.find(strJsonEnd) ) != std::string::npos) {
                    
                    int jsonStart = input.find(strJsonStart);

                    // find JSON start
                    if (jsonStart != std::string::npos) {

                        std::string rawJson = input.substr(jsonStart + strJsonStart.length(), jsonEnd - (jsonStart + strJsonStart.length()));
                        JsonDocument request;
                        DeserializationError error = deserializeJson(request, rawJson);

                        if (!error) {
                            StreamPointerWriter writer(stream);
                            serializeJsonPretty(processRequest(request), writer);
                        } else {
                            stream->print("deserializeJson() failed: ");
                            stream->println(error.f_str()); 
                            stream->println(rawJson.c_str());                           
                        }

                    }

                    // remove JSON from input buffer
                    input = input.substr(jsonEnd + strJsonEnd.length());
                }
            }

            vTaskDelay(1);

            // safe check if the stream was closed so the task needs to end
            if (taskParams->checkStreamClosed)
                if (taskParams->checkStreamClosed())
                    break;
        }

        Serial.println("Ending streamListenerTask");

        delete taskParams;
        vTaskDelete(NULL);
    }

    JsonDocument WebConfig::processRequest(JsonDocument &request) {

        JsonDocument response;

        std::string requestType = request["requestType"];
        //Serial.println(requestType.c_str());

        if (requestType == "getConfig") {

            ::BluepadHub.getStatusIndicator()->setEventPattern(bluepadhub::StatusIndicator::EventPattern::Info);
            
            //response["result"] = "success";
            //response["data"] = saveSettingsToJson();
            return saveSettingsToJson();
        }

        else if (requestType == "getDeviceInfo") {
            return getDeviceInfo();
        }

        else if (requestType == "updateConfig") {

            JsonDocument jsonConfig = request["requestPayload"];
            
            if (loadSettingsFromJson(jsonConfig)) {

                saveSettingsToStorage();
                applySettings();

                ::BluepadHub.getStatusIndicator()->setEventPattern(bluepadhub::StatusIndicator::EventPattern::Success);

                response["result"] = "success";
                response["message"] = "Config updated";

            } else {

                ::BluepadHub.getStatusIndicator()->setEventPattern(bluepadhub::StatusIndicator::EventPattern::Error);

                response["result"] = "error";
                response["message"] = "Invalid config";
            }
        }

        else {
            response["result"] = "error";
            response["message"] = "Unknown requestType";
        }

        return response;
    }    
}