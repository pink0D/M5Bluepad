//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "UniConfig.h"

#include <map>
#include <algorithm>


namespace bluepadhub {
    
    static const char* uniConfigVersion = "UniConfigV1";

    static const std::map<BluepadControllerButton, std::string> mapButtons = {
        {BluepadControllerButton::NullButton, ""}, 
        {BluepadControllerButton::DpadUp, "DpadUp"},
        {BluepadControllerButton::DpadDown, "DpadDown"},
        {BluepadControllerButton::DpadLeft, "DpadLeft"}, 
        {BluepadControllerButton::DpadRight, "DpadRight"}, 
        {BluepadControllerButton::L1, "L1"},
        {BluepadControllerButton::R1, "R1"}, 
        {BluepadControllerButton::L3, "L3"}, 
        {BluepadControllerButton::R3, "R3"}, 
        {BluepadControllerButton::Square, "Square"}, 
        {BluepadControllerButton::Triangle, "Triangle"}, 
        {BluepadControllerButton::Cross, "Cross"}, 
        {BluepadControllerButton::Circle, "Circle"}
    };

    static const std::map<BluepadControllerInput, std::string> mapInputs = {
        {BluepadControllerInput::NullInput, ""},
        {BluepadControllerInput::L2, "L2"},
        {BluepadControllerInput::R2, "R2"},
        {BluepadControllerInput::LeftStickX, "LeftStickX"}, 
        {BluepadControllerInput::LeftStickY, "LeftStickY"}, 
        {BluepadControllerInput::RightStickX, "RightStickX"}, 
        {BluepadControllerInput::RightStickY, "RightStickY"}
    };

    std::string mapBluepadControllerButtonToString(BluepadControllerButton btn) {

        if (mapButtons.count(btn) > 0)
            return mapButtons.at(btn);

        return "";
    }

    BluepadControllerButton mapStringToBluepadControllerButton(std::string str) {

        auto it = std::find_if(mapButtons.begin(), mapButtons.end(), 
            [&str](const std::pair<BluepadControllerButton, std::string>& pair) {
                return pair.second == str;
            });

        if (it != mapButtons.end()) 
            return it->first;

        return BluepadControllerButton::NullButton;
    }

    std::string mapBluepadControllerInputToString(BluepadControllerInput input) {

        if (mapInputs.count(input) > 0)
            return mapInputs.at(input);

        return "";
    }

    BluepadControllerInput mapStringToBluepadControllerInput(std::string str) {
        auto it = std::find_if(mapInputs.begin(), mapInputs.end(), 
            [&str](const std::pair<BluepadControllerInput, std::string>& pair) {
                return pair.second == str;
            });

        if (it != mapInputs.end()) 
            return it->first;

        return BluepadControllerInput::NullInput;
    }

    void UniConfig::loadDefaultSettings(int numMotors, int numServos) {

        deviceName = defaultDeviceName;

        for (int i=0; i<numMotors; i++) {
            
            MotorConfig motorConfig;

            motorConfig.minPWM = defaultMinPWM;
            motorConfig.maxPWM = 100;
            motorConfig.input = BluepadControllerInput::NullInput;
            motorConfig.brake = BluepadControllerInput::NullInput;
            motorConfig.invertInput = false;
            motorConfig.invertButton = BluepadControllerButton::NullButton;

            motor.push_back(motorConfig);
        }

        for (int i=0; i<numServos; i++) {

            ServoConfig servoConfig;

            servoConfig.maxAngle = 90;
            servoConfig.input = BluepadControllerInput::NullInput;
            servoConfig.invertInput = false;
            servoConfig.invertButton = BluepadControllerButton::NullButton;

            servo.push_back(servoConfig);
        }
    }

    JsonDocument UniConfig::saveSettingsToJson() {
        
        JsonDocument json, deviceSettings, motors, servos;

        deviceSettings["deviceName"] = deviceName;

        for (int i=0; i<motor.size(); i++) {

            MotorConfig motorConfig = motor[i];

            JsonDocument motor;
            motor["input"] = mapBluepadControllerInputToString(motorConfig.input);
            motor["brake"] =  mapBluepadControllerInputToString(motorConfig.brake);
            motor["invertInput"] = motorConfig.invertInput;
            motor["invertButton"] = mapBluepadControllerButtonToString(motorConfig.invertButton);
            //motor["minPWM"] = motorConfig.minPWM;
            //motor["maxPWM"] = motorConfig.maxPWM;

            motors[i] = motor;
        }

        for (int i=0; i<servo.size(); i++) {

            ServoConfig servoConfig = servo[i];

            JsonDocument servo;
            servo["input"] = mapBluepadControllerInputToString(servoConfig.input);
            servo["invertInput"] = servoConfig.invertInput;
            servo["invertButton"] = mapBluepadControllerButtonToString(servoConfig.invertButton);
            servo["maxAngle"] = servoConfig.maxAngle;

            servos[i] = servo;
        }    

        json["version"] = uniConfigVersion;
        json["deviceSettings"] = deviceSettings;
        json["motors"] = motors;
        json["servos"] = servos;

        return json;
    }

    bool UniConfig::loadSettingsFromJson(JsonDocument &json) {

        std::string version = json["version"];

        if (version != uniConfigVersion)
            return false;

        for (int i=0; i<motor.size(); i++) {

            MotorConfig motorConfig = motor[i];

            motorConfig.input = mapStringToBluepadControllerInput(json["motors"][i]["input"]);
            motorConfig.brake = mapStringToBluepadControllerInput(json["motors"][i]["brake"]);
            motorConfig.invertInput = json["motors"][i]["invertInput"];
            motorConfig.invertButton = mapStringToBluepadControllerButton(json["motors"][i]["invertButton"]);
            //motorConfig.maxPWM = max(0, min(100,       (int)(json["motors"][i]["maxPWM"])));
            //motorConfig.minPWM = max(0, min(motorConfig.maxPWM, (int)(json["motors"][i]["minPWM"])));

            motor[i] = motorConfig;
        }

        for (int i=0; i<servo.size(); i++) {

            ServoConfig servoConfig = servo[i];

            servoConfig.input = mapStringToBluepadControllerInput(json["servos"][i]["input"]);
            servoConfig.invertInput = json["servos"][i]["invertInput"];
            servoConfig.invertButton = mapStringToBluepadControllerButton(json["servos"][i]["invertButton"]);
            servoConfig.maxAngle = max(1, min(180,  (int)(json["servos"][i]["maxAngle"])));

            servo[i] = servoConfig;
        }

        return true;
    }


}
