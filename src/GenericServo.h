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

#include "ChannelFilter.h"

class GenericServoController {
    public:
        GenericServoController() {};
        virtual void outputServoPulse(int channel, uint16_t pulse) {};
};

class GenericServo : public ChannelFilter {

    public:
        GenericServo() {};

        void setController(GenericServoController* _controller, int _channel);
        
        void stop();

        void setServoPulseRange(uint16_t pulse_min, uint16_t pulse_max);
        void setServoMaxAngle(uint16_t angle_max);

        void updateServo(double normalized_position);


    protected:
        uint16_t servoPulseMin = 1000;
        uint16_t servoPulseMax = 2000; 
        
    private:
        GenericServoController* controller;
        int channel = 0;
};

#endif