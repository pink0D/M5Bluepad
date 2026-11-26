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
#include "MultiMotorController.h"
#include <MouldKingExtensions.h>

namespace bluepadhub::web {

    enum class MouldKingModuleType {None, MK40, MK40x3, MK60};

    class MouldKingModuleTypeMapperImpl: public EnumMapper<MouldKingModuleType> {
            void init() override {
                addMap(MouldKingModuleType::None,   "");
                addMap(MouldKingModuleType::MK40,   "MK40");
                addMap(MouldKingModuleType::MK40x3, "MK40x3");
                addMap(MouldKingModuleType::MK60,   "MK60");

                setDefaultEnum(MouldKingModuleType::None);
                setDefaultString("");
            };
    };
    extern MouldKingModuleTypeMapperImpl MouldKingModuleTypeMapper;

    class MouldKingController : public MultiMotorController {

        public:

            MouldKingController(std::string label)
            : MultiMotorController("MouldKingProfile", label) {
                reservedChannelCount = 12;
            };

            void enableMultiModule(bool enable = true) { multiModuleEnabled = enable; };
            void enableAdvancedChannels(bool enable = true) { advancedChannels = enable; };

            virtual void processBluepadController(Profile *profile, BluepadController *ctl);

            virtual void stop();


        protected:

            virtual int getChannelCount();
       
            virtual void loadDefaultSettings();
            virtual void applySettings();

            virtual bool loadSettingsFromJson(JsonDocument &json);
            virtual JsonDocument saveSettingsToJson();

        private:

            MouldKingModuleType moduleType;
            IMKModule *mk = nullptr;

            bool advancedChannels = false;
            bool multiModuleEnabled = false;            
    };

}

