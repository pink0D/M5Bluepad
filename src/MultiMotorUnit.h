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

    template <int numMotors, class MotorClass = GenericMotor>
    class MultiMotorUnit : private GenericMotorController {

        public:
            MultiMotorUnit() {  

                for (int i=0; i<numMotors; i++) {
                    motors[i].setController(this, i); 
                } 

                dummyMotor.setController(nullptr, -1); 
            };

            virtual void stopMotors() {
                for (int i=0; i<numMotors; i++) {
                    motor(i)->stop(); 
                } 
            };

            virtual void stop() {
                stopMotors();
            };
            
            inline MotorClass* motor(int channel) {
                if (channel < 0)
                    return &dummyMotor;
                    
                if (channel < numMotors)
                    return &motors[channel];

                return &dummyMotor;
            };

            void setMotorLimits(uint8_t channel, double limit_min, double limit_max) {
                motor(channel)->setLimits(limit_min, limit_max);
            };

            void updateMotorSpeed(uint8_t channel, double normalized_speed) {
                motor(channel)->updateSpeed(normalized_speed);
            };

            void updateMotor(uint8_t channel, double normalized_speed) {
                updateMotorSpeed(channel, normalized_speed);
            };

            void brakeMotor(uint8_t channel) {
                motor(channel)->brake();
            };

        private:
            MotorClass motors[numMotors];
            MotorClass dummyMotor;
    };

}

#endif