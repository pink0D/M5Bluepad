//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _CONFIG_COMPONENT_H
#define _CONFIG_COMPONENT_H


#include <Arduino.h>

#include <ArduinoJson.h>

#include <map>


namespace bluepadhub {

    template <class EnumClass>
    class EnumMapper {
        public:
            EnumMapper() {};

            void checkInit() {
                static bool done = false;
                if (!done) {
                    init();
                    done = true;
                }
            }

            std::string mapEnumToString(EnumClass e) {

                checkInit();

                if (m.count(e) > 0)
                    return m.at(e);

                return defaultString;
            };

            EnumClass mapStringToEnum(std::string s) {

                checkInit();

                auto it = std::find_if(m.begin(), m.end(), 
                    [&s](const std::pair<EnumClass, std::string>& pair) {
                        return pair.second == s;
                    });

                if (it != m.end()) 
                    return it->first;

                return defaultEnum;
            };

        protected:
            virtual void init() {};

            void addMap(EnumClass e, std::string s) {
                m[e] = s;
            };

            void setDefaultEnum(EnumClass e) {
                defaultEnum = e;
            };

            void setDefaultString(std::string s) {
                defaultString = s;
            };

        private:
            std::map<EnumClass,std::string> m;
            std::string defaultString;
            EnumClass defaultEnum;
    };

    class ConfigComponent {

        public:
            ConfigComponent(std::string name, std::string label, bool extension = false, bool enableProfiles = false) {
                this->name = name;
                this->label = label;
                this->extension = extension;      
                this->enableProfiles = enableProfiles;
            };

            void begin();
            void reload();

            std::string getName() { return name; };

            void setLabel(std::string label) { this->label = label; };
            std::string getLabel() { return label; };

            bool isInitialized() { return loadCompleted; };
            bool isExtension() { return extension; };
            bool hasProfiles() { return enableProfiles; };

            virtual bool isEnabled() { return true; };        
            
            // main overridable methods
            virtual void loadDefaultSettings() {};
            virtual void applySettings() {};

            virtual bool loadSettingsFromJson(JsonDocument &json) { return true; };
            virtual JsonDocument saveSettingsToJson() { return JsonDocument(); };

            // flash storage impementation, usually reusable. override if no flash storage is required
            virtual void loadSettingsFromStorage();
            virtual void saveSettingsToStorage();

            // static methods

            static void initFS();

            static void registerConfigComponent(std::string name, ConfigComponent* component);
            static void unregisterConfigComponent(std::string name);
            static ConfigComponent* getConfigComponent(std::string name);
            static void iterateComponents(const std::function<void(ConfigComponent*)>& callback);


        private:
            std::string name;
            std::string label;
            bool loadCompleted = false;
            bool extension = false;
            bool enableProfiles = false;

            String getConfigPath();

            static inline std::map<std::string, ConfigComponent*> configComponents;

    };

    class ExtensionComponent : public ConfigComponent {
        public:
            ExtensionComponent() : ConfigComponent("Extension", "Ext") {};

            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;

            // no write to flash - this is done inside extension components
            void loadSettingsFromStorage() override {};
            void saveSettingsToStorage() override {};
    };
}

#endif