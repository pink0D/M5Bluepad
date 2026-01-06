//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MULTI_SERVO_UNIT_H_
#define _MULTI_SERVO_UNIT_H_

#include "GenericServo.h"

namespace bluepadhub {

    template <int numServos, class ServoClass = GenericServo>
    class MultiServoUnit : private GenericServoController {

        public:
            MultiServoUnit() {

                for (int i=0; i<numServos; i++) {
                    servos[i].setController(this, i); 
                } 

                dummyServo.setController(nullptr, -1);            
            };

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

            void updateServo(uint8_t channel, double normalized_position) {
                servo(channel)->updateServo(normalized_position);
            };

        private:        
            ServoClass servos[numServos];
            ServoClass dummyServo;
    };

}

#endif