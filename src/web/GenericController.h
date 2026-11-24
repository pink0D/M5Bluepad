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
#include "BluepadHub.h"
#include "ConfigComponent.h"
#include "GenericChannel.h"

namespace bluepadhub {

    class GenericController : public ConfigComponent{

        public:
            GenericController(std::string name, std::string label);
            virtual ~GenericController();

            void processBluepadController(BluepadController *ctl);
            virtual void stop() {};

        protected:

            virtual int getChannelCount() { return 0; };
            virtual void setChannelDefaults(int channel, GenericChannel *settings) {};

            virtual void updateOutput(int channel, float value, bool brake) {};
            virtual void applyOutputSettings(int channel, GenericChannel *settings) {};

        private:

            virtual void loadDefaultSettings();
            virtual void applySettings();

            virtual bool loadSettingsFromJson(JsonDocument &json);
            virtual JsonDocument saveSettingsToJson();

            GenericChannel *channels = nullptr;

            
    };

}

