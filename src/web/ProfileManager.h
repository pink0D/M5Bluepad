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
    
    class ProfileManager : public ConfigComponent {

        public:
            ProfileManager() : ConfigComponent("ProfileManager", "ProfileManager", true, false) {};

            static String getActiveProfileDirectory();


        private:

            void loadDefaultSettings() override;
            void applySettings() override;

            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;

            static void createDefaultProfile();
            static bool removeDirectoryRecursive(std::string path);

            static inline std::string activeProfile;


    };    
}
