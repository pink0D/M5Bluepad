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

#include "ChannelFilter.h"

class GenericMotorController {
    public:
        GenericMotorController() {};

        virtual void outputMotorSpeed(int channel, double normalized_speed) {};

        virtual void outputMotorBrake(int channel) { 
            outputMotorSpeed(channel, 0); // default implementation is setting speed to zero
        };
};

class GenericMotor : public ChannelFilter {

    public:
        GenericMotor() {};

        void setController(GenericMotorController* _controller, int _channel);
        
        void stop();

        void setLimits(double limit_min, double limit_max);
        
        void updateSpeed(double normalized_speed);
        void brake();

        
    private:
        GenericMotorController* controller;
        int channel = 0;
};

#endif