//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ChangeoverCatch.h"

#include <esp_timer.h>

namespace bluepadhub
{

    void ChangeoverCatch::begin() {

        if (servo == nullptr) {
            Serial.println("WARNING: no servo was set for ChangeoverCatch");
            return;
        }

        servo->setServoAngles(angle_neutral, 180);
        servo->begin();
    }

    void ChangeoverCatch::update() {

        if (servo == nullptr) 
            return;

        long time_now = esp_timer_get_time();

        if (engaged) {

            if (engaged_timeout == 0) {
                engaged_timeout = time_now + move_timeout*1000;
            }

            if (time_now < engaged_timeout) {
                servo->setServoPosition(angle_engaged_forced);
            } else {
                servo->setServoPosition(angle_engaged);
            }

            neutral_timeout = 0;

        } else { // neutral
        
            if (neutral_timeout == 0) {
                neutral_timeout = time_now + move_timeout*1000;
            }

            if (time_now < neutral_timeout) {
                servo->setServoPosition(angle_neutral_forced);
            } else {
                servo->setServoPosition(angle_neutral);
            }

            engaged_timeout = 0;        
        }
        
    }

    void ChangeoverCatch::stop() {

        if (servo == nullptr) 
            return;

        servo->setServoPosition(angle_neutral);
        engaged = false;
        engaged_timeout = 0;
    }

}