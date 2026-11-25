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
#include "MultiMotorUnit.h"

namespace bluepadhub {

    class MultiMotorController : public GenericController{

        public:

            MultiMotorController(std::string label)
            : MultiMotorController(nullptr, label) {};

            MultiMotorController(IMultiMotorUnit &unit, std::string label) 
            : MultiMotorController(&unit, label) {};
            
            MultiMotorController(IMultiMotorUnit *unit, std::string label) 
            : GenericController("MotorProfile", label) {
                setUnit(unit);
            };
            
            void setUnit(IMultiMotorUnit *unit) {
                this->unit = unit;
            };

            void setUnit(IMultiMotorUnit &unit) {
                this->unit = &unit;
            };

            virtual void stop() {
                if (unit != nullptr)
                    unit->stopMotors();
            };

        protected:

            virtual int getChannelCount() {

                if (unit != nullptr)
                    return unit->getMotorCount();

                return 0;
            };

            virtual void updateOutput(int channel, float value, bool brake) {

                if (unit == nullptr)
                    return;

                if (brake)
                    unit->getMotor(channel)->brake();
                else
                    unit->getMotor(channel)->updateSpeed(value);

            };

            virtual void applyOutputSettings(int channel, GenericChannel *settings) {

                if (unit == nullptr)
                    return;

                unit->getMotor(channel)->setLimits( (float) settings->minPower / 100.0, (float) settings->maxPower / 100.0);
                unit->getMotor(channel)->setAccelerationTime(settings->softStartTime);
            };            

        private:
            IMultiMotorUnit *unit = nullptr;
    };

}

