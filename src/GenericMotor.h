//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <Arduino.h>
#include "OutputFilter.h"

#define MOTOR_1 0
#define MOTOR_2 1

#define MOTOR_A 0
#define MOTOR_B 1
#define MOTOR_C 2
#define MOTOR_D 3
#define MOTOR_E 4
#define MOTOR_F 5

namespace bluepadhub {

    class GenericMotorController {
        public:
            GenericMotorController() = default;
            virtual ~GenericMotorController() = default;

            virtual void outputMotorSpeed(int channel, float normalized_speed) {};

            virtual void outputMotorBrake(int channel) { 
                outputMotorSpeed(channel, 0); // default implementation is setting speed to zero
            };
    };

    class GenericMotor : public bluepadhub::OutputFilter {

        public:
            GenericMotor() = default;

            void setController(GenericMotorController* _controller, int _channel);
            
            void stop();

            void setLimits(float limit_min, float limit_max);
            
            void updateSpeed(float normalized_speed);
            void brake();

            
        private:
            GenericMotorController* controller = nullptr;
            int channel = -1;
    };

}
