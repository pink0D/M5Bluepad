//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _DUMBO_RC_LED_H_
#define _DUMBO_RC_LED_H_

#include "ServoPWM.h"

class DumboRCLED : public bluepadhub::ServoPWM {
    public:
        DumboRCLED() {};
        DumboRCLED(int pwm_pin) : bluepadhub::ServoPWM (pwm_pin) {};

        virtual void begin(int pwm_pin);
        virtual void begin() {
                bluepadhub::ServoPWM::begin();
            };
            
        virtual void stop();

        void toggleMode();
};


#endif