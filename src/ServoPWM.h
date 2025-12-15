//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _SERVO_PWM_H_
#define _SERVO_PWM_H_

#include <ESP32Servo.h>
#include "GenericServo.h"

namespace bluepadhub {

    class ServoPWM : public GenericServo, private Servo, private GenericServoController {

        public:
            //ServoPWM() {};
            ServoPWM(int pwm_pin) {
                this->pwm_pin = pwm_pin;
            };
            
            void begin(int pwm_pin);
            void begin() {
                begin(pwm_pin);
            }

        private:
            virtual void outputServoPulse(int channel, uint16_t pulse);  
            int pwm_pin = 0;      
    };
}

typedef bluepadhub::ServoPWM GeekServo;

#endif