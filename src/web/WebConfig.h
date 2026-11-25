//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _WEB_CONFIG_H_
#define _WEB_CONFIG_H_

#include "ConfigComponent.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "ConfigProfile.h"

namespace bluepadhub {

    class WebConfig {

        public:

            WebConfig() {};

            void setDefaultHostname(std::string hostname);
            void setDefaultSSID(std::string ssid);
            void setWiFiTaskDelay(long wifiTaskDelayMillis) {
                this->wifiTaskDelayMillis = wifiTaskDelayMillis;
            };

            void begin();

        public:
            WebServer webServer;
            WiFiManager wifiManager;

        private:

            long wifiTaskDelayMillis = 0;
            bool isResetting = false;
            
            void wifiStartupTask();
            void wifiConnectTask();

            void resetConfig();

    };

}

#endif