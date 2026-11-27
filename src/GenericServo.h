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

#define SERVO_1 0
#define SERVO_2 1
#define SERVO_3 2
#define SERVO_4 3
#define SERVO_5 4
#define SERVO_6 5

namespace bluepadhub {

    class GenericServoController {
        public:
            GenericServoController() = default;
            virtual ~GenericServoController() = default;

            virtual void outputServoPulse(int channel, uint16_t pulse) {};
    };

    class GenericServo : public bluepadhub::OutputFilter {

        public:
            GenericServo() = default;

            void setController(GenericServoController* _controller, int _channel);
            
            virtual void begin() {};
            virtual void stop();

            void setServoPulseRange(uint16_t pulse_min, uint16_t pulse_max);
            void setServoMaxAngle(uint16_t angle_max);
            void setServoAngles(int16_t angle_zero, uint16_t angle_max);            

            void updateServo(float normalized_position);
            void setServoPosition(int16_t angle_position);


        protected:
            uint16_t servoPulseMin = 1000;
            uint16_t servoPulseMax = 2000; 
            uint16_t servoPulseMiddle = 1500;
            uint16_t servoPulseWidth = 500;

            virtual void updateServoPulseRange() {};
            
        private:
            GenericServoController* controller = nullptr;
            int channel = -1;
    };

}
