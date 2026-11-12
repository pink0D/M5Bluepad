//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "DumboRCLED.h"

void DumboRCLED::begin(int pwm_pin) {
    bluepadhub::ServoPWM::begin(pwm_pin);
    stop();
}

void DumboRCLED::stop() {
    updateServo(-1.0);
}

void DumboRCLED::toggleMode() {

    xTaskCreate( [](void* ctx) {

        DumboRCLED* obj = static_cast<DumboRCLED*>(ctx);

        obj->updateServo(-1.0);
        vTaskDelay(pdMS_TO_TICKS(200));
        obj->updateServo(0.0);
        vTaskDelete(NULL);

      }, "taskLight", 8*1024, this, 0, nullptr);
}