//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _UNI_CONFIG_H_
#define _UNI_CONFIG_H_

#include "ConfigProfile.h"

#include <vector>
#include <string>
#include <ArduinoJson.h>


namespace bluepadhub {

    std::string mapBluepadControllerButtonToString(BluepadControllerButton btn);
    BluepadControllerButton mapStringToBluepadControllerButton(std::string str);

    std::string mapBluepadControllerInputToString(BluepadControllerInput input);
    BluepadControllerInput mapStringToBluepadControllerInput(std::string str);

    class UniConfig {  

        public:

            void loadDefaultSettings(int numMotors, int numServos);
            bool loadSettingsFromJson(JsonDocument &json);
            JsonDocument saveSettingsToJson(); 

            class MotorConfig {

                public:
                    MotorConfig() {};

                    int minPWM;
                    int maxPWM;

                    BluepadControllerInput input;
                    BluepadControllerInput brake;
                    
                    bool invertInput;
                    BluepadControllerButton invertButton;
            };

            class ServoConfig {

                public:
                    ServoConfig() {};

                    int maxAngle;

                    BluepadControllerInput input;
                    
                    bool invertInput;
                    BluepadControllerButton invertButton;
            };

            // defaults
            std::string defaultDeviceName = "BluepadHub";
            int defaultMinPWM = 0;


            // begin parameters

            std::string deviceName;

            std::vector<MotorConfig> motor;
            std::vector<ServoConfig> servo;

            // end parameters    
        };    

}

#endif