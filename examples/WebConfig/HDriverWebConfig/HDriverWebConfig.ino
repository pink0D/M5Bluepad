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
#include <light/DumboRCLED.h>
#include <web/WebConfig.h>
#include <string>

#include <web/TelegramBot.h>
// TelegramBot significantly increases the size of compiled sketch
// Before upload, set partition scheme to "Minimal SPIFFS (Large APPS with OTA)" 

M5AtomLiteIndicator   AtomLiteIndicator;
M5AtomLiteButton      AtomLiteButton;
M5AtomDeepSleep       AtomDeepSleep;

M5HDriverBaseExt HDriverBaseExt;                    // HDriverBase extension
M5GroveServo GroveServo(SERVO_ATOM_LITE_PORT_A1);   // Servo connected directy to Atom's grove port
DumboRCLED RCLed(SERVO_ATOM_LITE_PORT_A2);     // LED Servo 

class : public bluepadhub::Profile, public bluepadhub::WebConfig {

  int motorMinPWM;
  bool motorInvert;  
  bool motorBrake;

  bool servoInvert;
  int servoMaxAngle;

  bool simpleMode;

  bool autoPowerOn;
  bool autoPairing;

  String wifiSSID;
  String wifiPassword;
  String botToken;
  String chatId;
  String deviceName;

  void loadDefaultSettings() {

    motorMinPWM = 25;
    motorInvert = false;
    motorBrake = true;

    servoInvert = false;
    servoMaxAngle = 90;

    simpleMode = false;
    autoPowerOn = true;
    autoPairing = true;

    wifiSSID = "WIFI_SSID";
    wifiPassword = "WIFI_PASSWORD";
    botToken = "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    chatId = "XXXXXXXXX";
    deviceName = "M5Stack Atomic HDriver Base";
  };

  JsonDocument saveSettingsToJson() { 

    JsonDocument json;

    json["motorMinPWM"] = motorMinPWM;
    json["motorInvert"] = motorInvert;
    json["motorBrake"] = motorBrake;

    json["servoMaxAngle"] = servoMaxAngle;
    json["servoInvert"] = servoInvert;

    json["simpleMode"] = simpleMode;
    json["autoPowerOn"] = autoPowerOn;
    json["autoPairing"] = autoPairing;

    json["wifiSSID"] = wifiSSID;
    json["wifiPassword"] = wifiPassword;
    json["botToken"] = botToken;
    json["chatId"] = chatId;
    json["deviceName"] = deviceName;

    return json; 
  }; 

  bool loadSettingsFromJson(JsonDocument &json) { 

    //TODO: add validation
    
    motorMinPWM = json["motorMinPWM"];
    motorInvert = json["motorInvert"];
    motorBrake = json["motorBrake"];

    servoMaxAngle = json["servoMaxAngle"];
    servoInvert = json["servoInvert"];

    simpleMode = json["simpleMode"];
    autoPowerOn = json["autoPowerOn"];
    autoPairing = json["autoPairing"];

    wifiSSID = String((const char*)json["wifiSSID"]);
    wifiPassword = String((const char*)json["wifiPassword"]);
    botToken = String((const char*)json["botToken"]);
    chatId = String((const char*)json["chatId"]);
    deviceName = String((const char*)json["deviceName"]);

    return true;
  };

  JsonDocument getDeviceInfo() { 

      JsonDocument info;

      info["batteryVoltage"] = ((int)(HDriverBaseExt.getVoltage() * 10) / 10.0);
      info["numMotors"] = 1;
      info["numServos"] = 1;

      return info; 
  };

  void setup() {

    WebConfig::begin();

    enablePairingAfterStartup = autoPairing;

    if (autoPowerOn)
      AtomDeepSleep.setCheckWakeUpReason(false);

    AtomDeepSleep.begin();
    
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
        
    AtomLiteButton.begin();

    HDriverBaseExt.begin();

    GroveServo.begin();

    RCLed.begin();
  };

  void applySettings() {

    HDriverBaseExt.setLimits(motorMinPWM / 100, 1.0);

    static bool initialUpdate = true;

    if (!initialUpdate) {
        GroveServo.detach();
    }

    GroveServo.setServoMaxAngle(servoMaxAngle);

    if (!initialUpdate)
    {
        GroveServo.begin();
        GroveServo.updateServo(0);
    }   
    
    initialUpdate = false;  
  };

  void processBluepadController(BluepadController* ctl) {

    // Options button
    if (this->wasClicked(ctl->miscButtons() & 0x04))
        setDiscoverable(60);    

    double speed = normalizeTriggerInput(ctl->throttle());  // R trigger to accelerate
    double steer = normalizeStickInput(ctl->axisX());       // L stick to steer
    bool brake =  normalizeTriggerInput(ctl->brake()) > 0;  // L trigger to brake

    if (ctl->y() || ctl->r1()) // reverse while holding Y or R1
      speed = -speed;

    if (wasClicked(ctl->l1() && ctl->r1()))
      simpleMode =! simpleMode;

    if (simpleMode) {
      speed = -normalizeStickInput(ctl->axisY());
      steer = normalizeStickInput(ctl->axisRX()); 
    }

    if (motorInvert)
      speed = -speed;

    if (servoInvert)
      steer = -steer;

    GroveServo.updateServo(steer);

    if (brake && motorBrake) {
      HDriverBaseExt.brake();             
    }
    else {
      HDriverBaseExt.updateSpeed(speed);  
    }

    // toggle lights mode
    if (wasClicked(ctl->dpad() & 0x01)) {

      RCLed.toggleMode();
    }

    // lights off
    if (wasClicked(ctl->dpad() & 0x02)) {
      RCLed.stop();
    }
  };

  void failsafe() {
    GroveServo.stop();
    HDriverBaseExt.stop();
    RCLed.stop();
  };

  bool isLowBatteryState() {
    return HDriverBaseExt.isLowBattery();
  }

  bool isFaultState() {
    return HDriverBaseExt.isFault();
  }

  void idleTimer() {
    BluepadHub.startDeepSleep();
  }

  void deepSleepTimer() {

    Serial.println("Sending telegram notification");

    bluepadhub::TelegramBot bot;

    bot.setWiFiSSID(wifiSSID);
    bot.setWiFiPassword(wifiPassword);
    bot.setBotToken(botToken);
    bot.setChatId(chatId);
    bot.setDeviceName(deviceName);

    bot.sendVoltageStatus(HDriverBaseExt.getVoltage(), isLowBatteryState());
  }

} HDriverWebConfig;

void setup() {
  BluepadHub.begin();
}

void loop() {
  BluepadHub.update();
}
