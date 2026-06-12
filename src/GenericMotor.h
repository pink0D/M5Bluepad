//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _GENERIC_MOTOR_H_
#define _GENERIC_MOTOR_H_

#include <Arduino.h>
#include "OutputFilter.h"

namespace bluepadhub {

    class GenericMotorController {
        public:
            GenericMotorController() {};

            virtual void outputMotorSpeed(int channel, float normalized_speed) {};

            virtual void outputMotorBrake(int channel) { 
                outputMotorSpeed(channel, 0); // default implementation is setting speed to zero
            };
    };

    class GenericMotor : public bluepadhub::OutputFilter {

        public:
            GenericMotor() {};

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

#endif