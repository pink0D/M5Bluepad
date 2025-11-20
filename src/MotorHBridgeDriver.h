//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MOTOR_HBRIDGE_DRIVER_H_
#define _MOTOR_HBRIDGE_DRIVER_H_

#include <ESP32PWM.h>
#include "GenericMotor.h"

class MotorHBridgeDriver : public GenericMotor, private GenericMotorController {

    public:
        MotorHBridgeDriver() {};
        
        void begin(int pwm_pin1, int pwm_pin2, int pwm_frequency);

    private:
        virtual void outputMotorSpeed(int channel, double normalized_speed);
        virtual void outputMotorBrake(int channel);

        ESP32PWM pwm_c1;
        ESP32PWM pwm_c2;        
};

#endif