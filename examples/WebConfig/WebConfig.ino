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
#include <web/ProfileManager.h>

#include <web/TelegramBot.h>
#include <web/BluetoothSettings.h>

#include <web/MultiMotorController.h>
#include <web/MultiServoController.h>

#include "esp_bt.h"

#include "m5stack_files.h"

M5AtomLiteIndicator   AtomLiteIndicator;
M5AtomLiteButton      AtomLiteButton;
M5AtomDeepSleep       AtomDeepSleep;

bluepadhub::MultiMotorUnit<1, M5HDriverBaseExt> MotorUnit;
bluepadhub::MultiServoUnit<2, M5GroveServo> ServoUnit;

bluepadhub::WebConfig WebConfig;
bluepadhub::ProfileManager ProfileManager;

bluepadhub::TelegramBot TelegramBot;
bluepadhub::BluetoothSettings BluetoothSettings;

class : public bluepadhub::MultiMotorController {
  using bluepadhub::MultiMotorController::MultiMotorController;

  std::string getChannelLabel(int channel) override {
     return "DRV8876"; 
  };

} MotorController(MotorUnit, "HDriverProfile","HDriver");

class : public bluepadhub::MultiServoController {
  using bluepadhub::MultiServoController::MultiServoController;

  std::string getChannelLabel(int channel) override {

    if (channel == 0)
      return "Grove Servo 1 (Yellow wire)";

    if (channel == 1)
      return "Grove Servo 2 (White wire)";

    return ""; 
  };

} ServoController(ServoUnit, "ServoProfile", "Servo");


class : public bluepadhub::Profile {

  void setup() {

    AtomDeepSleep.setCheckWakeUpReason(true);
    AtomDeepSleep.begin();
    
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
        
    AtomLiteButton.begin();

    // WebConfig must be set up before any other configs
    WebConfig.webServer.addWebFiles(m5stack_web_data);
    WebConfig.setWiFiTaskDelay(1000);
    WebConfig.begin();

    BluetoothSettings.begin();
    TelegramBot.begin();

    MotorController.begin();
    ServoController.begin();

    // ProfileManager must be set up last so it loads configs for all other components
    ProfileManager.begin();

    // set up physical devices after their config has been updated during profile loading

    MotorUnit.motor(0)->begin(); 

    ServoUnit.servo(0)->begin(SERVO_ATOM_LITE_PORT_A1);
    ServoUnit.servo(1)->begin(SERVO_ATOM_LITE_PORT_A2);
  };

  void processBluepadController(BluepadController* ctl) {
    MotorController.processBluepadController(ctl);
    ServoController.processBluepadController(ctl);
  };

  void failsafe() {    
    MotorController.stop();
    ServoController.stop();
  };

  bool isLowBatteryState() {
    return MotorUnit.motor(0)->isLowBattery();
  };

  bool isFaultState() {
    return MotorUnit.motor(0)->isFault();
  };

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

    TelegramBot.sendVoltageStatus(MotorUnit.motor(0)->getVoltage(), isLowBatteryState());
  };

  void idleTimer() {
    // check web server activity before going to sleep mode (5 minutes)
    if (WebConfig.webServer.getLastActivityTime() < esp_timer_get_time() - 5*60*1000*1000) {
        BluepadHub.startDeepSleep();
    } else {
      Serial.println("WebServer is active, sleep not started");
    }
  }; 

} WebConfigProfile;


void setup() {
  BluepadHub.begin();
}

void loop() {
  BluepadHub.update();
}
