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

    class TelegramBot : public ConfigComponent {

        public:
            TelegramBot() : ConfigComponent("TelegramBot", "TelegramBot", true) {};

            void setWiFiSSID(const std::string& _wifi_ssid) {
                this->wifi_ssid = _wifi_ssid;
            };

            void setWiFiPassword(const std::string& _wifi_password) {
                this->wifi_password = _wifi_password;
            };

            void setBotToken(const std::string& _bot_token) {
                this->bot_token = _bot_token;
            };

            void setChatId(const std::string& _chat_id) {
                this->chat_id = _chat_id;
            };

            void setDeviceName(const std::string& _device_name) {
                this->device_name = _device_name;
            };

            void sendVoltageStatus(float voltage, bool isLowVoltage);
            void sendMessage(const String& text, const String& parse_mode);

            bool isEnabled() override { return enabled; };   

        private:
            std::string wifi_ssid;
            std::string wifi_password;
            std::string bot_token;
            std::string chat_id;
            std::string device_name;
            bool enabled = false;

            void loadDefaultSettings() override;

            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;
            
    };

}
