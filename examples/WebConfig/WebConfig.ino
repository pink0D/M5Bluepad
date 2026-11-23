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
//#include <web/TelegramBot.h>

#include "m5stack_files.h"


M5AtomLiteIndicator   AtomLiteIndicator;
M5AtomLiteButton      AtomLiteButton;
//M5AtomDeepSleep       AtomDeepSleep;

M5HDriverBaseExt HDriverBaseExt;                   // HDriverBase extension
M5GroveServo GroveServo(SERVO_ATOM_LITE_PORT_A);   // Servo connected directy to Atom's grove port

bluepadhub::WebConfig webConfig;

//bluepadhub::TelegramBot bot;


class : public bluepadhub::Profile {

  // this method is implicitly called after controller startup
  void setup() {

    //AtomDeepSleep.setCheckWakeUpReason(true);
    //AtomDeepSleep.begin();
    
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
        
    AtomLiteButton.begin();

    enablePairingAfterStartup = true;

    HDriverBaseExt.begin(); 

    GroveServo.begin();

    webConfig.webServer.addWebFiles(m5stack_web_data);
    //webConfig.setWiFiTaskDelay(1000);
    webConfig.begin();

    //bot.begin();
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

  // NOT WORKING - memory issues during TLS connection
  // need more optimizations in WebConfig memory usage
  /*

  void deepSleepTimer() {

    Serial.println("Sending telegram notification");

    bot.begin();

    if (!bot.isEnabled()) {
      Serial.println("TelegramBot not enabled by config");
      return;
    }

    if (!webConfig.wifiManager.connect(false, false)) {
      Serial.println("Could not connect to WiFi network to reach Telegram");
      return;
    }

    bot.sendVoltageStatus(HDriverBaseExt.getVoltage(), isLowBatteryState());
  }

  void idleTimer() {
    // check web server activity before going to sleep mode (5 minutes)
    if (webConfig.webServer.getLastActivityTime() < esp_timer_get_time() - 5*60*1000*1000) {
        BluepadHub.startDeepSleep();
    } else {
      Serial.println("WebServer is active, sleep not started");
    }
  }

  */

} WebConfigProfile;


void setup() {
  BluepadHub.begin();
}

void loop() {
  BluepadHub.update();
}
