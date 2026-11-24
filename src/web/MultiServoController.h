//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <Arduino.h>
#include "GenericController.h"
#include "MultiServoUnit.h"

namespace bluepadhub {

    class MultiServoController : public GenericController{

        public:
            MultiServoController(std::string name, std::string label) : GenericController(name, label) {};
            MultiServoController(IMultiServoUnit *unit, std::string name, std::string label) : GenericController(name, label) {
                setUnit(unit);
            };
            MultiServoController(IMultiServoUnit &unit, std::string name, std::string label) : GenericController(name, label) {
                setUnit(unit);
            };
            
            virtual ~MultiServoController() {};

            void setUnit(IMultiServoUnit *unit) {
                this->unit = unit;
            };
            void setUnit(IMultiServoUnit &unit) {
                this->unit = &unit;
            };

            virtual void stop() {
                if (unit != nullptr)
                    unit->stopServos();
            };

        protected:

            virtual int getChannelCount() {
                if (unit != nullptr)
                    return unit->getServoCount();

                return 0;
            };

            virtual void updateOutput(int channel, float value, bool brake) {
                if (unit == nullptr)
                    return;

                unit->getServo(channel)->updateServo(value);            
            };

            virtual void applyOutputSettings(int channel, GenericChannel settings) {
                if (unit == nullptr)
                    return;

                if (settings.servoUnits == ServoUnits::Micros) {
                    unit->getServo(channel)->setServoPulseRange(settings.servoMin, settings.servoMax);
                }

                if (settings.servoUnits == ServoUnits::Angle) {
                    unit->getServo(channel)->setServoAngles(settings.servoCenter, settings.servoMaxAngle);
                }
            };            

        private:
            IMultiServoUnit *unit = nullptr;
    };

}

