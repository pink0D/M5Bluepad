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

namespace bluepadhub {

  void TelegramBot::sendVoltageStatus(double voltage, bool isLowVoltage) {

    String mac = WiFi.macAddress();

    String msg = "";

    if (device_name.length() > 0) {
      msg += "<b>" + device_name + "</b>\n";
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

    WiFiClientSecure client;
    UniversalTelegramBot bot(bot_token, client);

    Serial.println("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    
    while (WiFi.status() != WL_CONNECTED) {
      vTaskDelay(pdMS_TO_TICKS(500));
    }

    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    bot.sendMessage(chat_id, text, parse_mode);

    Serial.println("TelegramBot: message sent");  
  }

}