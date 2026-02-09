//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _GENERIC_SERVO_H_
#define _GENERIC_SERVO_H_

#include <Arduino.h>
#include "OutputFilter.h"

namespace bluepadhub {

    class GenericServoController {
        public:
            GenericServoController() {};
            virtual void outputServoPulse(int channel, uint16_t pulse) {};
    };

    class GenericServo : public bluepadhub::OutputFilter {

        public:
            GenericServo() {};

            void setController(GenericServoController* _controller, int _channel);
            
            virtual void stop();

            void setServoPulseRange(uint16_t pulse_min, uint16_t pulse_max);
            void setServoMaxAngle(uint16_t angle_max);

            void updateServo(double normalized_position);


        protected:
            uint16_t servoPulseMin = 1000;
            uint16_t servoPulseMax = 2000; 
            uint16_t servoPulseMiddle = 1500;
            uint16_t servoPulseWidth = 500;
            
        private:
            GenericServoController* controller = nullptr;
            int channel = -1;
    };

}

#endif