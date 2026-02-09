//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _LIGHT_CONTROLLER_H_
#define _LIGHT_CONTROLLER_H_

namespace bluepadhub
{
    class LightController {

        public: 

            struct State {
                                
                bool DRL = false;
                bool headlights = false;

                bool leftSignal = false;        
                bool rightSignal = false;
                bool signalBlink = false;
                 
                bool brake = false;
            };

            void enableAutoTurnSignals(bool enable = true) { 
                autoTurnSignals = enable; 
            };

            void enableAutoBrakeSignals(bool enable = true) { 
                autoBrakeSignals = enable; 
            };

            void enableHardBrakeBlinking(bool enable = true) {
                hardBrakeBlinking = enable;
            };

            void setAutoTurnSignalsMinSteer(double value) {
                autoTurnSignalsMinSteer = value;
            };

            void setBrakeTimeoutMillis(int timeout) {
                brakeTimeout = timeout;
            };

            virtual void begin() {};
            
            void stop() {

                DRL = false;
                headlights = false;
                hazardLights = false;
                leftSignal = false; 
                rightSignal = false;

                State s;
                s.DRL = false;
                s.headlights = false;
                s.brake = false;
                s.leftSignal = false;
                s.rightSignal = false;
                s.signalBlink = false;

                updateLights(s);
            };

            void update(double steer, double speed, bool brake);

            void toggleDRL() { 
                DRL = !DRL; 
            };
            void setDRL(bool state) {
                DRL = state; 
            };

            void toggleHeadlights() { 
                headlights = !headlights; 
            };
            void setHeadlights(bool state) { 
                headlights = state; 
            };

            void toggleHazardLights() { 
                setHazardLights(!hazardLights); 
            };
            void setHazardLights(bool state, int count = 0) { 
                hazardLights = state; 
                leftSignal = false; 
                rightSignal = false; 
                blinkCount = count;

                resetBlinkTimeout();
            };
            void blinkhazardLights(int count = 3) {
                setHazardLights(true, count);
            };

            void toggleLeftTurnSignal() { 
                setLeftTurnSignal(!leftSignal); 
            };
            void setLeftTurnSignal(bool state, int count = 0) {                
                if (hazardLights)
                    return;

                leftSignal = state; 
                rightSignal = false; 
                hazardLights = false;
                blinkCount = count;

                resetBlinkTimeout();
            };
            void blinkLeftTurnSignal(int count = 3) { 
                setLeftTurnSignal(true, count); 
            };

            void toggleRightTurnSignal() { 
                setRightTurnSignal(!rightSignal); 
            };
            void setRightTurnSignal(bool state, int count = 0) {                 
                if (hazardLights)
                    return;
                    
                rightSignal = state; 
                leftSignal = false; 
                hazardLights = false;
                blinkCount = count;

                resetBlinkTimeout();
            };
            void blinkRightTurnSignal(int count = 3) { 
                setRightTurnSignal(true, count); 
            };

        protected:

            virtual void updateLights(State &state) {};

        private:
            
            bool autoTurnSignals = false;
            bool autoBrakeSignals = false;
            bool hardBrakeBlinking = false;

            double autoTurnSignalsMinSteer = 0.5;

            unsigned long brakeTimeout = 1000;
            unsigned long autoBrakeTimeout;
            unsigned long hardBrakeTimeout = 0;

            State state;

            bool DRL = false;
            bool headlights = false;
            bool hazardLights = false;
            bool leftSignal = false; 
            bool rightSignal = false;

            int blinkCount = 0;
            bool signalBlink = false;
            unsigned long blinkTimeout = 0;

            double speed = 0;
            double steer = 0;
            bool brake = false;


            void resetBlinkTimeout() {
                // start new blink immediately if no signals are active
                if ( (!state.leftSignal) && (!state.rightSignal) ) {
                    blinkTimeout = 0;
                    signalBlink = false;
                }
            };

    };
    
} 

#endif