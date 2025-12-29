//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _TELEGRAM_BOT_H_
#define _TELEGRAM_BOT_H_

#include <Arduino.h>

namespace bluepadhub {

    class TelegramBot {

        public:
            TelegramBot() {};

            void setWiFiSSID(const String& _wifi_ssid) {
                this->wifi_ssid = _wifi_ssid;
            };

            void setWiFiPassword(const String& _wifi_password) {
                this->wifi_password = _wifi_password;
            };

            void setBotToken(const String& _bot_token) {
                this->bot_token = _bot_token;
            };

            void setChatId(const String& _chat_id) {
                this->chat_id = _chat_id;
            };

            void setDeviceName(const String& _device_name) {
                this->device_name = _device_name;
            };

            void sendVoltageStatus(double voltage, bool isLowVoltage);
            void sendMessage(const String& text, const String& parse_mode);

        private:
            String wifi_ssid;
            String wifi_password;
            String bot_token;
            String chat_id;
            String device_name;
            
    };

}

#endif