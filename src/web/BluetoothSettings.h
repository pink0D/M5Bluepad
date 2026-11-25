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
#include "ConfigComponent.h"

namespace bluepadhub::web {

    class BluetoothSettings : public ConfigComponent {

        public:
            BluetoothSettings() : ConfigComponent("BluetoothSettings", "BluetoothSettings", true) {};

        private:
            
            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;
            
    };

}

