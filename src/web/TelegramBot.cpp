//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "TelegramBot.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

namespace bluepadhub::web {

  void TelegramBot::sendVoltageStatus(float voltage, bool isLowVoltage) {

    String mac = WiFi.macAddress();

    String msg = "";

    if (device_name.length() > 0) {
      msg += "<b>" + String(device_name.c_str()) + "</b>\n";
    }

    if (isLowVoltage) {
      msg += "\xF0\x9F\x9B\x9C " + mac + " \xF0\x9F\xAA\xAB " + String(voltage, 1) + "V\n";
      msg += "\xE2\x9D\x97 <b>LOW BATTERY</b> \xE2\x9D\x97\n";
    } else {
      msg += "\xF0\x9F\x9B\x9C " + mac + " \xF0\x9F\x94\x8B " + String(voltage, 1) + "V\n";
    }

    sendMessage(msg, "HTML");
  }

  void TelegramBot::sendMessage(const String& text, const String& parse_mode) {

    if (isInitialized()) {
      // component was initialized as a part of Config system, so rely on external WiFiManager

      // check that component is enabled
      if (!enabled)
      {
        Serial.println("TelegramBot not enabled by config");
        return;
      }

    } else { 
      // connect inside this method

      Serial.println("Connecting to WiFi...");

      WiFi.mode(WIFI_STA);
      WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

      while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
      }

      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }  

    WiFiClientSecure client;
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

    UniversalTelegramBot bot(bot_token.c_str(), client);   

    if (bot.sendMessage(chat_id.c_str(), text, parse_mode)) {
      Serial.println("TelegramBot: message sent");      
    } else {
      Serial.println("TelegramBot: ERROR while sending message");      
    }
    
  }

  void TelegramBot::loadDefaultSettings() {
    enabled = false;
    device_name = "M5Bluepad";
    chat_id = "";
    bot_token = "";
  }

  bool TelegramBot::loadSettingsFromJson(JsonDocument &json) {

    enabled = json["enabled"];
    device_name = json["deviceName"].as<std::string>(); 
    chat_id = json["chatId"].as<std::string>(); 
    bot_token = json["botToken"].as<std::string>(); 

    return true;
  }

  JsonDocument TelegramBot::saveSettingsToJson() {

    JsonDocument json;

    json["enabled"] = enabled;
    json["deviceName"] = device_name;
    json["chatId"] = chat_id;
    json["botToken"] = bot_token;

    return json;
  }


}