//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include "ServoPWM.h"

namespace bluepadhub::ext
{
    class DumboRCLED : public bluepadhub::ServoPWM {
        public:
            DumboRCLED() = default;
            DumboRCLED(int pwm_pin) : bluepadhub::ServoPWM (pwm_pin) {};

            virtual void begin(int pwm_pin);
            virtual void begin() {
                    bluepadhub::ServoPWM::begin();
                };
                
            virtual void stop();

            void toggleMode();
    };
}
