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
#include <LittleFS.h>

namespace bluepadhub {

    void WebConfig::setDefaultHostname(std::string hostname) {
        wifiManager.setDefaultHostname(hostname);
    }

    void WebConfig::setDefaultSSID(std::string ssid) {
        wifiManager.setDefaultSSID(ssid);
    }

    void WebConfig::begin() {

        ConfigComponent::initFS();

        xTaskCreate( [](void* ctx) {    
            static_cast<WebConfig*>(ctx)->wifiStartupTask();
        }, "wifiStartupTask", TASK_STACK_SIZE, this, 0, NULL);

    }

    void WebConfig::wifiStartupTask() {

        // delay on startup to wait until power button is released normally
        vTaskDelayMillis(wifiTaskDelayMillis);

        // enable WiFi if button is pressed during startup or if there was no config FS during startup
        if (ConfigComponent::isFirstStartup() || ::BluepadHub.getMultiFunctionButton()->isPressed()) {

            // connect to wifi in async task so it does not delay possible hard reset event
            xTaskCreate( [](void* ctx) {    
                static_cast<WebConfig*>(ctx)->wifiConnectTask();
            }, "wifiConnectTask", TASK_STACK_SIZE, this, 0, NULL);
        }

        long timeout = 10000;
        while (timeout > 0) {

            // end task if button is released before hard reset timeout
            if (::BluepadHub.getMultiFunctionButton()->isPressed() == false) {
                vTaskDelete(NULL);
                return;
            }

            timeout -= 100;
            vTaskDelayMillis(100);
        }        

        // if the button is still pressed then perform a hard reset
        resetConfig();

        vTaskDelete(NULL);
    }

    void WebConfig::wifiConnectTask() {
      
        wifiManager.connect();
        webServer.start();

        while (true) {
            vTaskDelayMillis(5000);
            if (!isResetting) {
                if (wifiManager.isActiveAP()) {
                    ::BluepadHub.getStatusIndicator()->setEventPattern(StatusIndicator::EventPattern::WiFi_AP);
                } else {
                    ::BluepadHub.getStatusIndicator()->setEventPattern(StatusIndicator::EventPattern::WiFi_STA);
                }
            }
        }
            
    }

    void WebConfig::resetConfig() {

        isResetting = true;

        Serial.println("Resetting Bluetooth pairing");
        ::BluepadHub.resetPairing();

        Serial.println("Formatting LittleFS...");

        if (LittleFS.format()) {
            Serial.println("LittleFS formatted successfully.");
        } else {
            Serial.println("LittleFS format failed.");
        } 

        vTaskDelayMillis(3000);
        Serial.println("Rebooting");
        esp_restart();

    }

    
}