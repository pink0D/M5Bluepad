//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "DeepSleep.h"
#include "BluepadHub.h"

#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>

namespace bluepadhub {

    void DeepSleep::begin(bool setBluepadHubDeepSleep) {
        
        if (setBluepadHubDeepSleep) 
            ::BluepadHub.setDeepSleep(this);

        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {

            Serial.println("Wake up by timer");

            xTaskCreate( [](void* ctx) {

                static_cast<DeepSleep*>(ctx)->task();

            }, "DeepSleep", 8*1024, this, 0, nullptr);

            vTaskDelayMillis(::BluepadHub.getProfile()->deepSleepTaskMaxDuration);

            Serial.println("Deep sleep task time limit");
            startDeepSleep(); // back to deep sleep

        } else {
            Serial.println("Startup...");
            afterStartup();
        }
    }

    void DeepSleep::startDeepSleep() {
        
        Serial.println("Starting deep sleep"); 
        beforeSleep();        
    }

    void DeepSleep::task() {
        // execute profile implemented task
        if (::BluepadHub.getProfile() != nullptr) {
            ::BluepadHub.getProfile()->deepSleepTimer();
        }
        startDeepSleep(); // back to deep sleep
    }
}
