//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MULTI_MOTOR_UNIT_H_
#define _MULTI_MOTOR_UNIT_H_

#include "GenericMotor.h"

namespace bluepadhub {

    template <int numMotors>
    class MultiMotorUnit : private GenericMotorController {

        public:
            MultiMotorUnit() {        
                for (int i=0; i<numMotors; i++) {
                    motors[i].setController(this, i); 
                } 
                dummyMotor.setController(nullptr, -1); 
            };
            
            void stopMotors() {
                for (int i=0; i<numMotors; i++) {
                    motors[i].stop(); 
                } 
            };

            void stop() {
                stopMotors();
            };
            
            GenericMotor* motor(int channel) {
                if (channel < 0)
                    return &dummyMotor;
                    
                if (channel < numMotors)
                    return &motors[channel];

                return &dummyMotor;
            };

            void setMotorLimits(uint8_t channel, double limit_min, double limit_max) {
                if (channel >= 0 && channel < numMotors) {
                    motors[channel].setMotorLimits(limit_min, limit_max);
                }
            };

            void updateMotorSpeed(uint8_t channel, double normalized_speed) {
                if (channel >= 0 && channel < numMotors) {
                    motors[channel].updateSpeed(normalized_speed);
                }
            };

            void brakeMotor(uint8_t channel) {
                if (channel >= 0 && channel < numMotors) {
                    motors[channel].brake();
                }
            };

        private:
            GenericMotor motors[numMotors];
            GenericMotor dummyMotor;
    };

}

#endif