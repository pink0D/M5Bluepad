//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <BluepadHub.h>
#include <M5Extensions.h>

#include <web/WebConfig.h>
#include <web/BluetoothSettings.h>
#include <web/ProfileManager.h>

#include <web/TelegramBot.h>

#include "esp_bt.h"

#include "m5stack_files.h"


M5AtomLiteIndicator   AtomLiteIndicator;
M5AtomLiteButton      AtomLiteButton;
M5AtomDeepSleep       AtomDeepSleep;

M5HDriverBaseExt HDriverBaseExt;                   
M5GroveServo GroveServo(SERVO_ATOM_LITE_PORT_A);   

bluepadhub::WebConfig WebConfig;
bluepadhub::BluetoothSettings BluetoothSettings;
bluepadhub::ProfileManager ProfileManager;

bluepadhub::TelegramBot TelegramBot;

class : public bluepadhub::Profile {

  void setup() {

    AtomDeepSleep.setCheckWakeUpReason(true);
    AtomDeepSleep.begin();
    
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
        
    AtomLiteButton.begin();

    HDriverBaseExt.begin(); 

    GroveServo.begin();

    WebConfig.webServer.addWebFiles(m5stack_web_data);
    WebConfig.setWiFiTaskDelay(1000);
    WebConfig.begin();

    BluetoothSettings.begin();
    ProfileManager.begin();

    TelegramBot.begin();
  };

  void processBluepadController(BluepadController* ctl) {


  };

  void failsafe() {
    GroveServo.stop();
    HDriverBaseExt.stop();
  };

  bool isLowBatteryState() {
    return HDriverBaseExt.isLowBattery();
  }

  bool isFaultState() {
    return HDriverBaseExt.isFault();
  }


  void deepSleepTimer() {

    // free memory allocated for bluetooth stack 
    // TLS connection to Telegram API requires lots of free heap 
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
      esp_bt_controller_disable();
    }
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_IDLE ) {
      esp_bt_controller_deinit();
    }
    esp_bt_mem_release(ESP_BT_MODE_BTDM);

    Serial.println("Sending telegram notification");

    TelegramBot.begin();

    if (!TelegramBot.isEnabled()) {
      Serial.println("TelegramBot not enabled by config");
      return;
    }

    if (!WebConfig.wifiManager.connect(false, false)) {
      Serial.println("Could not connect to WiFi network to reach Telegram");
      return;
    }

    TelegramBot.sendVoltageStatus(HDriverBaseExt.getVoltage(), isLowBatteryState());
  }

  void idleTimer() {
    // check web server activity before going to sleep mode (5 minutes)
    if (WebConfig.webServer.getLastActivityTime() < esp_timer_get_time() - 5*60*1000*1000) {
        BluepadHub.startDeepSleep();
    } else {
      Serial.println("WebServer is active, sleep not started");
    }
  } 

} WebConfigProfile;


void setup() {
  BluepadHub.begin();
}

void loop() {
  BluepadHub.update();
}
