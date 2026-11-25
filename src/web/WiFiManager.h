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

#include <list>

#include "ConfigComponent.h"

namespace bluepadhub::web {

    class WiFiManager : public ConfigComponent {

        public:
            WiFiManager() : ConfigComponent("WiFiManager", "WiFi") {};

            void setDefaultHostname(std::string hostname);
            void setDefaultSSID(std::string ssid);

            // canCreateAP - start an access point when no network is available
            // startMDNS   - advertise hostname.local for the web interface; pass false for
            //               outbound-only connections (e.g. Telegram notification on deep-sleep
            //               wake up) to save the internal RAM that mDNS task and buffers use,
            //               which the TLS handshake needs
            bool connect(bool canCreateAP = true, bool startMDNS = true);      
            
            std::string getHostname() {
                return hostname;
            };

            int getRSSI();
            bool isActiveAP() { return apMode; };
            
        private:

            struct WiFiNetwork {
                std::string ssid;
                std::string password;
            };

            std::list<WiFiNetwork> networks;
            WiFiNetwork ap;

            std::string defaultHostname = "m5bluepad";
            std::string defaultSSID = "M5BLUEPAD";

            std::string hostname;
            int connectTimeout;

            bool reboot;

            bool apMode = false;

            void loadDefaultSettings() override;
            void applySettings() override;

            bool loadSettingsFromJson(JsonDocument &json) override;
            JsonDocument saveSettingsToJson() override;

            std::string generateDefaultSSID();
            std::string filterString(const std::string& input, char c_min, char c_max, int max_len = 32);
            bool isValidHostname(const std::string& s);
    };
}
