//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _CHANGEOVER_CATCH_H_
#define _CHANGEOVER_CATCH_H_

#include "GenericServo.h"

namespace bluepadhub
{
    class ChangeoverCatch {

        public: 

            ChangeoverCatch() {};
            ChangeoverCatch(GenericServo& _servo) {
                this->servo = &_servo;
            };

            void begin(GenericServo& _servo) {
                begin(&_servo);
            };
            void begin(GenericServo *_servo) {
                this->servo = _servo;
                begin();
            };

            void setAngles(int angleNeutral, int angleEngaged) {
                setAngles(angleNeutral, angleNeutral, angleEngaged, angleEngaged);
            };

            void setAngles(int angleNeutral, int angleNeutralForced, int angleEngaged, int angleEngagedForced, long forceTimeoutMillis) {
                this->angle_neutral = angleNeutral;
                this->angle_neutral_forced = angleNeutralForced;
                this->angle_engaged = angleEngaged;
                this->angle_engaged_forced = angleEngagedForced;
                this->move_timeout = forceTimeoutMillis;
            }

            void begin();
            void update();
            void stop();

            void setState(bool engaged){
                this->engaged = engaged;
            };

            void moveNeutral() {
                setState(false);
            };
            void moveEngaged() {
                setState(true);
            };
            

        private:
            
            GenericServo *servo = nullptr;

            bool engaged = false;
            long neutral_timeout = -1;
            long engaged_timeout = 0;
            long move_timeout = 0;

            int angle_neutral = 0;
            int angle_neutral_forced = 0;
            int angle_engaged = 0;
            int angle_engaged_forced = 0;
    };
    
} 

#endif