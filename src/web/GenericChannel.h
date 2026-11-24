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
#include <ArduinoJson.h>
#include "ConfigProfile.h"
#include "ConfigComponent.h"

namespace bluepadhub {

    enum class ChannelType {Disabled, Analog, Buttons, Stepper};

    class ChannelTypeMapperImpl: public EnumMapper<ChannelType> {
            void init() override {
                addMap(ChannelType::Disabled, "");
                addMap(ChannelType::Analog,   "Analog");
                addMap(ChannelType::Buttons,  "Buttons");
                addMap(ChannelType::Stepper,  "Stepper");

                setDefaultEnum(ChannelType::Disabled);
                setDefaultString("");
            };
    };
    extern ChannelTypeMapperImpl ChannelTypeMapper;

    enum class ServoUnits {Micros, Angle};

    class ServoUnitsMapperImpl: public EnumMapper<ServoUnits> {
            void init() override {
                addMap(ServoUnits::Micros, "micros");
                addMap(ServoUnits::Angle,  "angle");

                setDefaultEnum(ServoUnits::Micros);
                setDefaultString("micros");
            };
    };
    extern ServoUnitsMapperImpl ServoUnitsMapper;

    class BluepadControllerInputMapperImpl: public EnumMapper<BluepadControllerInput> {
            void init() override {
                addMap(BluepadControllerInput::NullInput, "");
                addMap(BluepadControllerInput::LeftStickX, "LeftStickX");
                addMap(BluepadControllerInput::LeftStickY, "LeftStickY");
                addMap(BluepadControllerInput::RightStickX, "RightStickX");
                addMap(BluepadControllerInput::RightStickY, "RightStickY");
                addMap(BluepadControllerInput::LeftTrigger, "LeftTrigger");
                addMap(BluepadControllerInput::RightTrigger, "RightTrigger");

                setDefaultEnum(BluepadControllerInput::NullInput);
                setDefaultString("");
            };
    };    
    extern BluepadControllerInputMapperImpl BluepadControllerInputMapper;

    class BluepadControllerButtonMapperImpl: public EnumMapper<BluepadControllerButton> {
            void init() override {
                addMap(BluepadControllerButton::NullButton, "");
                addMap(BluepadControllerButton::Cross, "Cross");
                addMap(BluepadControllerButton::Circle, "Circle");
                addMap(BluepadControllerButton::Square, "Square");
                addMap(BluepadControllerButton::Triangle, "Triangle");
                addMap(BluepadControllerButton::L1, "L1");
                addMap(BluepadControllerButton::R1, "R1");
                addMap(BluepadControllerButton::L2, "L2");
                addMap(BluepadControllerButton::R2, "R2");
                addMap(BluepadControllerButton::Share, "Share");
                addMap(BluepadControllerButton::Options, "Options");
                addMap(BluepadControllerButton::L3, "L3");
                addMap(BluepadControllerButton::R3, "R3");
                addMap(BluepadControllerButton::DPadUp, "DPadUp");
                addMap(BluepadControllerButton::DPadDown, "DPadDown");
                addMap(BluepadControllerButton::DPadLeft, "DPadLeft");
                addMap(BluepadControllerButton::DPadRight, "DPadRight");

                setDefaultEnum(BluepadControllerButton::NullButton);
                setDefaultString("");
            };
    };  
    extern BluepadControllerButtonMapperImpl BluepadControllerButtonMapper;
    
    class GenericChannel  {

        public:
            GenericChannel() {};

            void getOutput(BluepadController *ctl, float *value, bool* brake);

            void loadDefaultSettings();
            void applySettings();

            bool loadSettingsFromJson(JsonObject json);
            JsonDocument saveSettingsToJson();


            // settings

            ChannelType type;
            BluepadControllerInput input;
            bool invertInput;
            BluepadControllerButton reverseButton;
            BluepadControllerInput brake;
            int brakeTimeout;
            BluepadControllerButton button1;
            BluepadControllerButton button2;
            BluepadControllerButton buttonStop;
            bool sticky;
            int steps;
            int minPower;
            int maxPower;
            ServoUnits servoUnits;
            int servoMin;
            int servoMax;
            int servoMaxAngle;
            int servoCenter;

            // state variables
                        
    };

}

