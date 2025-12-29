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
            MultiMotorUnit(bool createMotors = true) {  

                if (createMotors) {

                    motors = new GenericMotor[numMotors];

                    for (int i=0; i<numMotors; i++) {
                        motors[i].setController(this, i); 
                    } 
                }

                dummyMotor.setController(nullptr, -1); 
            };

            ~MultiMotorUnit() {
                if (motors != nullptr) {
                    delete [] motors;
                }
            }
            
            virtual void stopMotors() {
                for (int i=0; i<numMotors; i++) {
                    motor(i)->stop(); 
                } 
            };

            virtual void stop() {
                stopMotors();
            };
            
            GenericMotor* motor(int channel) {
                if (channel < 0)
                    return &dummyMotor;
                    
                if (channel < numMotors)
                    return accessMotor(channel);

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

        protected:
        
            virtual GenericMotor* accessMotor(uint8_t channel) {

                if (motors != nullptr)
                    return &motors[channel];
                
                return &dummyMotor;
            };

        private:
            GenericMotor *motors = nullptr;
            GenericMotor dummyMotor;
    };

}

#endif