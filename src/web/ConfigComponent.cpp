//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ConfigComponent.h"
#include "ProfileManager.h"

#include <FS.h>
#include <LittleFS.h>

/*
void printTree(fs::FS &fs, const char *dirname, uint8_t levels, String indent = "") {
  File root = fs.open(dirname);
  if (!root || !root.isDirectory()) {
    Serial.println("Failed to open directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("%s[DIR]  %s\n", indent.c_str(), file.name());
      if (levels > 0) {
        // file.path() returns the full path needed for recursion on ESP32
        printTree(fs, file.path(), levels - 1, indent + "  ");
      }
    } else {
      Serial.printf("%s[FILE] %s (%u bytes)\n", indent.c_str(), file.name(), file.size());
    }
    file = root.openNextFile();
  }
}*/

namespace bluepadhub {

    /*
    STATIC MEMBERS
    */

    void ConfigComponent::initFS() {

        static bool fs_init_done = false;

        if (!fs_init_done) {

            if (!LittleFS.begin(true)) {
                
                Serial.println("WARNING: LittleFS begin Failed");                
                return;
            }

            firstStartup = !directoryExists("/config");

            ConfigComponent::createDir("/config");
            
            fs_init_done = true;

            //Serial.println("LITTLEFS");
            //printTree(LittleFS, "/", 5);
        }
    }

    bool ConfigComponent::isFirstStartup() {

        ConfigComponent::initFS();
        
        return ConfigComponent::firstStartup;
    }

    bool ConfigComponent::directoryExists(const char *path) {

        File dir = LittleFS.open(path);
        if (!dir) {
            return false; // Path does not exist
        }

        bool isDir = dir.isDirectory();
        dir.close();
        return isDir;
    }

    void ConfigComponent::createDir(std::string dir) {
        
        LittleFS.mkdir(dir.c_str());

        std::string path = dir + "/.d";
        File file = LittleFS.open(path.c_str(), "a"); 
        if (!file) {
            Serial.print("Failed to touch file in new dir ");
            Serial.println(dir.c_str());
            return;
        }
        file.close();
    }

    void ConfigComponent::registerConfigComponent(std::string name, ConfigComponent* component) {

        configComponents[name] = component;

        // ensure Extention component is created as top level component
        if (component->isExtension()) {
            static ExtensionComponent ext;
            ext.begin();
        }
    }

    void ConfigComponent::unregisterConfigComponent(std::string name) {
        
        configComponents.erase(name);
    }

    ConfigComponent* ConfigComponent::getConfigComponent(std::string name) {

        if (configComponents.count(name) > 0)
            return configComponents.at(name);

        return nullptr;
    }

    void ConfigComponent::iterateComponents(const std::function<void(ConfigComponent*)>& callback) {

        for (const auto& [_, value] : configComponents) {
            callback(value);
        }
    }    

    /*
    INSTANCE METHODS
    */

    void ConfigComponent::begin() {       

        ConfigComponent::registerConfigComponent(name, this);

        
        if (hasProfiles()) {
            // config will be reloaded when ProfileManager initializes

        } else {
            // load config for components without profile support
            reload();
        }
    }

    void ConfigComponent::reload() {

        loadDefaultSettings();
        loadSettingsFromStorage();
        applySettings();    
        
        loadCompleted = true;
    }   

    String ConfigComponent::getConfigPath() {

        String configRoot = "/config";

        if (enableProfiles)
            configRoot = ProfileManager::getActiveProfileDirectory();

        return configRoot + String("/") + String(name.c_str()) + String(".json");
    }

    void ConfigComponent::loadSettingsFromStorage() {

        ConfigComponent::initFS();

        File file = LittleFS.open(getConfigPath(), FILE_READ);
        if (!file) {
            Serial.print("WARNING: Failed to open config file for reading: ");
            Serial.print(getConfigPath());
            Serial.println(" - default config will be loaded");
            return;
        }

        JsonDocument json; 

        DeserializationError error = deserializeJson(json, file);

        file.close();

        if (error) {
            Serial.println(getConfigPath());
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        loadSettingsFromJson(json);
    };
    
    void ConfigComponent::saveSettingsToStorage() {
        String path = getConfigPath();
        File file = LittleFS.open(path, FILE_WRITE, true);
        if (!file) {
            Serial.print("WARNING: Failed to open config file for writing: ");
            Serial.println(path);
            return;
        }

        if (serializeJson(saveSettingsToJson(), file) == 0) {
            Serial.print("Failed to write to config file: ");
            Serial.println(path);
        }

        file.close();
    };

    /*
    Extension component
    */

    bool ExtensionComponent::loadSettingsFromJson(JsonDocument &json) {

        bool result = true;

        if (json.is<JsonObject>()) {
            for (JsonPair kv : json.as<JsonObject>()) {

                ConfigComponent* component = ConfigComponent::getConfigComponent(kv.key().c_str());

                if (component == nullptr)
                    continue;

                JsonDocument jsonComp;
                jsonComp.set(kv.value());

                if (component->loadSettingsFromJson(jsonComp)) {

                    component->saveSettingsToStorage();
                    component->applySettings();

                } else {
                    result = false;
                }      
            }
        } else {
            result = false; 
        }

        return result;
    }

    JsonDocument ExtensionComponent::saveSettingsToJson() {

        JsonDocument json;

        auto callback = [&json](ConfigComponent* comp) {
            // list only extension components
            if (comp->isExtension()) {
                JsonDocument jsonComp = comp->saveSettingsToJson();
                json[comp->getName().c_str()] = jsonComp;
            }
        };        

        ConfigComponent::iterateComponents(callback);

        return json;
    }


}