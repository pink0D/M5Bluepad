//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include "GenericMotor.h"

namespace bluepadhub {

    class IMultiMotorUnit {
        public:
            virtual ~IMultiMotorUnit() = default;
            
            virtual int getMotorCount() = 0;
            virtual GenericMotor* getMotor(int channel) = 0;
            virtual void stopMotors() = 0;
    };

    template <int numMotors, class MotorClass = GenericMotor>
    class MultiMotorUnit : public IMultiMotorUnit, private GenericMotorController {

        public:
            MultiMotorUnit() {  

                for (int i=0; i<numMotors; i++) {
                    motors[i].setController(this, i); 
                } 

                dummyMotor.setController(nullptr, -1); 
            };

            virtual ~MultiMotorUnit() = default;

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

            void setMotorLimits(uint8_t channel, float limit_min, float limit_max) {
                motor(channel)->setLimits(limit_min, limit_max);
            };

            void updateMotorSpeed(uint8_t channel, float normalized_speed) {
                motor(channel)->updateSpeed(normalized_speed);
            };

            void updateMotor(uint8_t channel, float normalized_speed) {
                updateMotorSpeed(channel, normalized_speed);
            };

            void brakeMotor(uint8_t channel) {
                motor(channel)->brake();
            };

            virtual int getMotorCount() {
                return numMotors;
            };

            virtual GenericMotor* getMotor(int channel) {
                return motor(channel);
            };

        private:
            MotorClass motors[numMotors];
            MotorClass dummyMotor;
    };

}
