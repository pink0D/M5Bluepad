//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ProfileManager.h"
#include "ConfigComponent.h"

#include <FS.h>
#include <LittleFS.h>

namespace bluepadhub {

    /*
    STATIC MEMBERS
    */

    String ProfileManager::getActiveProfileDirectory() {

        return String("/config/profiles/") + String(activeProfile.c_str());
    }


    bool ProfileManager::removeDirectoryRecursive(std::string path) {

        File dir = LittleFS.open(path.c_str());

        if (!dir) {
            return false;
        }

        if (!dir.isDirectory()) {
            dir.close();
            return false;
        }

        File file = dir.openNextFile();
        while (file) {
            // Store the full path before closing the file handle
            std::string  filePath = file.path();
            bool isDir = file.isDirectory();
            file.close(); // Crucial: close handle before deleting to avoid "Has open FD" errors

            if (isDir) {
                if (!ProfileManager::removeDirectoryRecursive(filePath)) {
                    dir.close();
                    return false;
                }
            } else {
                if (!LittleFS.remove(filePath.c_str())) {
                    dir.close();
                    return false;
                }
            }
            file = dir.openNextFile();
        }

        dir.close();
        return LittleFS.rmdir(path.c_str());
    }    

    void ProfileManager::createDefaultProfile() {

        ConfigComponent::createDir("/config/profiles");
        ConfigComponent::createDir("/config/profiles/Default");
    }

    /*
    INSTANCE METHODS
    */

    void ProfileManager::loadDefaultSettings() {

        ConfigComponent::initFS();

        // ensure Default profile always exists
        createDefaultProfile();

        activeProfile = "Default";
    }

    void ProfileManager::applySettings() {

        // ensure Default profile always exists - create new empty dir it was just deleted
        createDefaultProfile();       

        auto callback = [](ConfigComponent* comp) {
            // reload components which support profiles
            if (comp->hasProfiles()) {
                comp->reload();
            }
        };        

        ConfigComponent::iterateComponents(callback);
    }
  
    bool ProfileManager::loadSettingsFromJson(JsonDocument &json) {

        activeProfile = "Default";
        
        JsonArray profiles = json["profiles"].as<JsonArray>();

        for (JsonObject profile : profiles) {

            std::string name = profile["name"].as<std::string>();
            std::string name_new = profile["name_new"].as<std::string>();

            bool active_flag = profile["active"];
            bool delete_flag = profile["delete"];
            bool rename_flag = profile["rename"];

            std::string dir =       "/config/profiles/" + name;                       
            std::string dir_new =   "/config/profiles/" + name_new;                       

            if (active_flag) {
                activeProfile = name;
            }

            if (delete_flag) {

                ProfileManager::removeDirectoryRecursive(dir);

                if (activeProfile == name)
                    activeProfile = "Default";

            } else if (rename_flag) {

                LittleFS.rename(dir.c_str(), dir_new.c_str());
                activeProfile = name_new;

            } else {
                ConfigComponent::createDir(dir.c_str());
            }
        }

        return true;
    }

    JsonDocument ProfileManager::saveSettingsToJson() {

        JsonDocument json;       
        JsonArray profiles = json["profiles"].to<JsonArray>();        

        File profilesRoot = LittleFS.open("/config/profiles");

        if (!profilesRoot || !profilesRoot.isDirectory()) {
            
            Serial.println("Failed to open profiles directory or path is not a directory");

        } else {

            File entry = profilesRoot.openNextFile();
            while (entry) {
                if (entry.isDirectory()) {

                    std::string profileName = entry.name();
                    bool active_flag = (profileName.compare(activeProfile) == 0);

                    JsonObject item = profiles.add<JsonObject>();

                    item["name"] = profileName;
                    item["active"] = active_flag;

                }
                entry.close();
                entry = profilesRoot.openNextFile();
            }
            profilesRoot.close();
        }

        return json;
    }


}