//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include "GenericServo.h"

namespace bluepadhub {

    class IMultiServoUnit {
        public:
            virtual ~IMultiServoUnit() = default;

            virtual int getServoCount() = 0;
            virtual GenericServo* getServo(int channel) = 0;
            virtual void stopServos() = 0;
    };

    template <int numServos, class ServoClass = GenericServo>
    class MultiServoUnit : public IMultiServoUnit, private GenericServoController {

        public:
            MultiServoUnit() {

                for (int i=0; i<numServos; i++) {
                    servos[i].setController(this, i); 
                } 

                dummyServo.setController(nullptr, -1);            
            };

            virtual ~MultiServoUnit() = default;

            virtual void stopServos() {
                for (int i=0; i<numServos; i++) {
                    servo(i)->stop(); 
                } 
            };

            virtual void stop() {
                stopServos();
            };
            
            inline ServoClass* servo(int channel) {
                if (channel < 0)
                    return &dummyServo;

                if (channel < numServos)
                    return &servos[channel];

                return &dummyServo;
            };

            void setServoPulseRange(uint8_t channel, uint16_t pulse_min, uint16_t pulse_max) {
                servo(channel)->setServoPulseRange(pulse_min, pulse_max);
            };

            void setServoMaxAngle(uint8_t channel, uint16_t angle_max) {
                servo(channel)->setServoMaxAngle(angle_max);
            };

            void setServoAngles(uint8_t channel, int16_t angle_zero, uint16_t angle_max) {
                servo(channel)->setServoAngles(angle_zero, angle_max);
            };

            void updateServo(uint8_t channel, float normalized_position) {
                servo(channel)->updateServo(normalized_position);
            };

            virtual int getServoCount() {
                return numServos;
            };

            virtual GenericServo* getServo(int channel) {
                return servo(channel);
            };

        private:        
            ServoClass servos[numServos];
            ServoClass dummyServo;
    };

}
