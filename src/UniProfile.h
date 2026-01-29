//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _UNI_PROFILE_H
#define _UNI_PROFILE_H

#include "WebConfig.h"
#include "UniConfig.h"

namespace bluepadhub {

    template<class BaseClass>
    class UniProfile : public BaseClass, private WebConfig {

        public:

            void setDefaultDeviceName(std::string defaultDeviceName) {
                config.defaultDeviceName = defaultDeviceName;
            };

            void setDefaultMinPWM(int defaultMinPWM) {
                config.defaultMinPWM = defaultMinPWM;
            };

            void begin() {
                ::BluepadHub.begin();
            };

            void update() {
                ::BluepadHub.update();
            };

        private:
            UniConfig config;

            void setup()  override final {

                WebConfig::begin();

                BaseClass::enablePairingAfterStartup = true;

                BaseClass::setup();
            };

            void processBluepadController(BluepadController* ctl) override final {

                // Options button
                if (this->wasClicked(ctl->miscButtons() & 0x04))
                    setDiscoverable(60);

                for (int i=0; i<this->getNumMotors(); i++) {

                    if (config.motor[i].input == BluepadControllerInput::NullInput)
                        continue;

                    double input = this->getInputValue(ctl, config.motor[i].input);
                    double brake = this->getInputValue(ctl, config.motor[i].brake);

                    if (config.motor[i].invertInput)
                        input = -input;

                    if (this->getButtonValue(ctl, config.motor[i].invertButton))
                        input = -input;

                    if (brake)
                        this->brakeMotor(i);
                    else
                        this->updateMotor(i, input);
                }

                for (int i=0; i<this->getNumServos(); i++) {

                    if (config.servo[i].input == BluepadControllerInput::NullInput)
                        continue;

                    double input = this->getInputValue(ctl, config.servo[i].input);

                    if (config.servo[i].invertInput)
                        input = -input;

                    if (this->getButtonValue(ctl, config.servo[i].invertButton))
                        input = -input;

                    this->updateServo(i, input);
                }

                BaseClass::processBluepadController(ctl);
            };


            void applySettings() {

                static bool initialUpdate = true;

                setDeviceName(config.deviceName);

                for (int i=0; i<this->getNumMotors(); i++) {

                    this->setMotorLimits(i,   ((double)config.motor[i].minPWM) / 100.0, 
                                            ((double)config.motor[i].maxPWM) / 100.0);
                    
                    if (!initialUpdate) {   
                        this->updateMotor(i, 0);
                    }
                }

                for (int i=0; i<this->getNumServos(); i++) {

                    if (!initialUpdate) {
                        this->servo(i)->detach();
                    }
                    
                    this->servo(i)->setServoMaxAngle(config.servo[i].maxAngle);
                    
                    if (!initialUpdate)
                    {
                        this->servo(i)->begin();
                        this->updateServo(i, 0);
                    }
                }

                initialUpdate = false;        
            };

            void loadDefaultSettings() {
                config.loadDefaultSettings(this->getNumMotors(), this->getNumServos());
            };

            bool loadSettingsFromJson(JsonDocument &json) { 
                return config.loadSettingsFromJson(json); 
            };

            JsonDocument saveSettingsToJson() { 
                return config.saveSettingsToJson(); 
            }; 

            JsonDocument getDeviceInfo() { 

                JsonDocument info;

                info["batteryVoltage"] = ((int)(this->getVoltage() * 10) / 10.0);
                info["numMotors"] = this->getNumMotors();
                info["numServos"] = this->getNumServos();

                return info; 
            };


    };

    
}

#endif