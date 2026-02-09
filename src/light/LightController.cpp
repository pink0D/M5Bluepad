//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "LightController.h"

#include <Arduino.h>

namespace bluepadhub
{
    void LightController::update(double steer, double speed, bool brake) {

        unsigned long time = millis();

        // auto updates

        if (autoTurnSignals) {

            if ( (steer <= -autoTurnSignalsMinSteer) && ( abs(this->steer) < autoTurnSignalsMinSteer) )
                blinkLeftTurnSignal();

            if ( (steer >=  autoTurnSignalsMinSteer) && ( abs(this->steer) < autoTurnSignalsMinSteer) )
                blinkRightTurnSignal();

        }

        if (abs(speed) > 0.01) {
            // reset timeout
            autoBrakeTimeout = 0;
        } else if (abs(this->speed) > 0.01 )  {
            // speed was just set to zero
            autoBrakeTimeout = time + brakeTimeout;
        }

        state.brake = autoBrakeSignals && (time < autoBrakeTimeout);


        // indication updates

        state.DRL = DRL;
        state.headlights = headlights;

        state.signalBlink = this->signalBlink;

        if (time > blinkTimeout) {

            blinkTimeout = time + 500;
            state.signalBlink = !(this->signalBlink);

            // update signal state before new blink
            if (state.signalBlink) {

                state.leftSignal = leftSignal || hazardLights;
                state.rightSignal = rightSignal || hazardLights;            
            }
        }
       
        // check if blink count was set
        if (blinkCount > 0) {

            // signal blink was just ended, check if blinking should continue
            if (this->signalBlink && (!state.signalBlink)) {

                blinkCount--;

                // reset state after last blink
                if (blinkCount == 0) {
                    this->leftSignal = state.leftSignal = false;
                    this->rightSignal = state.rightSignal = false;   
                    this->hazardLights = false;               
                }
            }
        }

        if (brake && (!this->brake)) {
            if ( ( (time < autoBrakeTimeout) || (abs(speed) > 0.01) ) && (hardBrakeTimeout == 0) ) {

                hardBrakeTimeout = time + brakeTimeout;
            }
        } 

        // rest hard brake blinking after acceleration
        if ( (!brake) && (abs(speed) > 0.01) )
            hardBrakeTimeout = 0;

        // blinking hard brake indication
        if (time < hardBrakeTimeout) {
            state.brake = ((time % 200) < 100) || (!hardBrakeBlinking);
        } else {
            // constant brake indication after timeout
            state.brake = state.brake || brake; // auto-brake OR brake
            hardBrakeTimeout = 0;
        }

        // set indication state
        updateLights(state);

        // save values for next update
        this->steer = steer;
        this->speed = speed;
        this->brake = brake;
        this->signalBlink = state.signalBlink;
    }
} 

