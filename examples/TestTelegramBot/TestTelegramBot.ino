//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <TelegramBot.h>

//  Arduino setup function
void setup() {
    Serial.begin(115200);

    bluepadhub::TelegramBot bot;

    // a step by step guide on creating your own Telegram bot can be found here:
    // https://randomnerdtutorials.com/telegram-esp32-motion-detection-arduino/

    bot.setWiFiSSID("WIFI_SSID");
    bot.setWiFiPassword("WIFI_PASSWORD");
    bot.setBotToken("XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    bot.setChatId("XXXXXXXXX");
    bot.setDeviceName("M5 Atom Lite");

    bot.sendMessage("Hello from <b>M5Bluepad</b> \xF0\x9F\xA4\x96","HTML");
}

// Arduino loop function
void loop() {
  delay(1000);
}
