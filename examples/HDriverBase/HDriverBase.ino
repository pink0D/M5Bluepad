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

#include <web/TelegramBot.h>
// TelegramBot significantly increases the size of compiled sketch
// Before upload, set partition scheme to "Minimal SPIFFS (Large APPS with OTA)" 


M5AtomLiteIndicator   AtomLiteIndicator;
M5AtomLiteButton      AtomLiteButton;
M5AtomDeepSleep       AtomDeepSleep;

M5HDriverBaseExt HDriverBaseExt;                   // HDriverBase extension
M5GroveServo GroveServo(SERVO_ATOM_LITE_PORT_A);   // Servo connected directy to Atom's grove port


// this class handles controller input ans sets channel outputs
class : public bluepadhub::Profile {

  // this method is implicitly called after controller startup
  void setup() {


    // if Deep Sleep is used, AtomDeepSleep.begin() must be called before any other initialization
    // when battery is connected, the controller goes to deep sleep mode - click Atom Lite button to for wake up
    AtomDeepSleep.setCheckWakeUpReason(true);
    AtomDeepSleep.begin();
    
    // WARNING: 
    // Deep Sleep only lowers power consumption for ESP32 chip
    // It does not disconnect the power supply!
    // Many other components remain powered (DC buck converter, LDO, DRV8876, CH552T, built-in LED in HDriver base)
    // Power consumption in deep sleep mode is about 15mA, which will drain a 450mAh battery in just ~30 hours
    // Since there's no hardware protection, this will discharge it below minimum safe level

    
    // use Atom Lite RGB LED for status indication
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
        
    // click Atom button for Deep Sleep / Wake Up
    // hold button to enable pairing, continue holding to forget paired devices  
    AtomLiteButton.begin();


    // uncomment to automatically start pairing every time the controller is started

    // enablePairingAfterStartup = true;


    // uncomment to adjust controller sensivity
    //
    // controllerStickDeadzoneLow = 50;        // 0 = lowest value for stick input
    // controllerStickDeadzoneHigh = 500;      // 512 = highest value for stick input
    // controllerTriggerDeadzoneLow = 5;       // 0 = lowest value for trigger input
    // controllerTriggerDeadzoneHigh = 1000;   // 1024 = highest value for trigger input 

    // actual deadzone values depend on type of controller used
    // TestBluePad32 example can be used to analyze raw values sent by controller


    // some DC motors start running only at 25% duty cycle.
    // max duty can be also specified to limit motor power
    HDriverBaseExt.setLimits(0.25, 1.0);

    // uncomment to enable debug output for voltage monitor  
    // HDriverBaseExt.printVoltage(true);

    HDriverBaseExt.begin(); 

    // specify pulse range for a 360-servo
    // without this call, the default range (1000..2000) will be used for a 180-servo
    // GroveServo.setServoPulseRange(500, 2500);  

    // limit servo rotation to 75 degrees (pulse range is calculated internally)
    GroveServo.setServoMaxAngle(75);

    GroveServo.begin();
  };

  // process updates from controller
  void processBluepadController(BluepadController* ctl) {

    // normalizeStickInput/normalizeTriggerInput methods apply deadzone correction to raw values
    // then integer values from controller are mapped to normalized range (-1.0, 1.0) 

    double speed = normalizeTriggerInput(ctl->throttle());  // R trigger to accelerate
    double steer = normalizeStickInput(ctl->axisX());       // L stick to steer
    bool brake =  normalizeTriggerInput(ctl->brake()) > 0;  // L trigger to brake

    if (ctl->y() || ctl->r1()) // reverse while holding Y or R1
      speed = -speed;

    // update servo position
    GroveServo.updateServo(steer);

    if (brake) {
      // HDriver supports fast braking for connected DC motor
      HDriverBaseExt.brake();             
    }
    else {
      // update motor speed: value > 0 (forward), value < 0 (reverse), value = 0 (coast)
      HDriverBaseExt.updateSpeed(speed);  
    }

    // for more details on controller data processing see TestBluePad32 example and Bluepad32 docs
  };

  // failsafe() is called 
  // 1) when no data is received from controller after timeout
  // 2) when low battery level is detected

  void failsafe() {
    GroveServo.stop();
    HDriverBaseExt.stop();
  };

  //
  // WARNING
  //
  // HDriver can only monitor battery voltage
  // When battery is low, failsafe() is called
  //
  // CONTROLLER IS STILL POWERED AND IDLE CURRENT CAN DRAIN AND DAMAGE THE BATTERY
  // ALWAYS DISCONNECT THE BATTERY WHEN CONTROLLER IS NOT USED
  //

  // 2S/3S voltage range is detected automatically
  bool isLowBatteryState() {
    return HDriverBaseExt.isLowBattery();
  }

  // HDriver has fault state indication for overcurrent / overload / over temperature
  bool isFaultState() {
    return HDriverBaseExt.isFault();
  }

  // this method is called after 5 minutes of inactivity (no controller input and no motor output)
  void idleTimer() {
    // go to deep sleep
    BluepadHub.startDeepSleep();
  }

  // this method is called every 30 minutes when the controller is in deep sleep mode
  // (but sometimes ESP32 misses deep sleep timer wakeup so this can periodically happen in 60..120 minutes)
  void deepSleepTimer() {

    // send telegram notification that the controller is still powered on using battery
    Serial.println("Sending telegram notification");

    bluepadhub::TelegramBot bot;

    // check the TestTelegramBot example and test your bot without waiting for deep sleep timer
    bot.setWiFiSSID("WIFI_SSID");
    bot.setWiFiPassword("WIFI_PASSWORD");
    bot.setBotToken("XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    bot.setChatId("XXXXXXXXX");
    bot.setDeviceName("M5Stack Atomic HDriver Base");

    bot.sendVoltageStatus(HDriverBaseExt.getVoltage(), isLowBatteryState());
  }

} MyProfile;
// global class instance - default Profile constructor sets itself as BluepadHub profile



//  Arduino setup function
void setup() {
  BluepadHub.begin();
}

// Arduino loop function
void loop() {
  BluepadHub.update();
  //no delay here because it's inside BluepadHub.update()
}
