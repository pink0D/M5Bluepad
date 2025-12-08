//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

//
// M5 Atom Lite is used together with M5 HDriver Base
// M5 HDriver Base has an internal voltage regulator 
// This combo is powered by Mould King Power Module via a Power Functions connector
// ESP32 starts after MK Module is powered on, then they are automatically paired via Bluetooth 
// 
// 1. The gamepad is connected to M5 Atom Lite (ESP32)
// 2. Input is processed by ESP32
// 3. ESP32 directly outputs servo signal to a PWM-pin in Atom Lite's grove port 
//    (grove-to-servo adapter is needed to connect a GeekServo)
// 4. Power Functions motors are connected to the Mould King module, 
//    which is controlled by ESP32 via Bluetooth protocol
//


#include <BluepadHub.h>
#include <M5Extensions.h>
#include <MouldKing.h>

M5AtomLiteIndicator   AtomLiteIndicator;
M5AtomLiteButton      AtomLiteButton;
M5HDriverBaseExt      HDriverBaseExt;              // HDriverBase is used as a power source for servo
M5GroveServo GroveServo(SERVO_ATOM_LITE_PORT_A);   // Servo connected directy to Atom's grove port

// MK 4.0 module
MouldKing40 MK;   //MK 6.0 is also supported (see MouldKing60 class)

// this class handles controller input ans sets channel outputs
class : public bluepadhub::Profile {

  // this method is implicitly called after controller startup
  void setup() {
  
    // use Atom Lite RGB LED for status indication
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
        
    // click Atom button for Deep Sleep / Wake Up
    // hold button to enable pairing, continue holding to forget paired devices  
    AtomLiteButton.begin();

    // uncomment to adjust controller sensivity
    //
    // controllerStickDeadzoneLow = 50;        // 0 = lowest value for stick input
    // controllerStickDeadzoneHigh = 500;      // 512 = highest value for stick input
    // controllerTriggerDeadzoneLow = 5;       // 0 = lowest value for trigger input
    // controllerTriggerDeadzoneHigh = 1000;   // 1024 = highest value for trigger input 

    // actual deadzone values depend on type of controller used
    // BluePad32 example can be used to analyze raw values sent by controller

    // specify pulse range for a 360-servo
    // without this call, the default range (1000..2000) will be used for a 180-servo
    // GroveServo.setServoPulseRange(500, 2500);  

    // limit servo rotation to 75 degrees (pulse range is calculated internally)
    GroveServo.setServoMaxAngle(75);
    GroveServo.begin();

    // initialize the HDriverBase so it can check supply voltage and prevent running servo from USB power
    HDriverBaseExt.begin(); 

    // setup the MK module
    MK.begin();
  };

  // this method is called after BLE initialization is complete
  // since MK module is controlled over Bluetooth, connect() method must be here, not in setup()
  void afterSetup() {
    MK.connect();
  };  

  // process updates from controller
  void processBluepadController(BluepadController* ctl) {

    // normalizeStickInput/normalizeTriggerInput methods apply deadzone correction to raw values
    // then integer values from controller are mapped to normalized range (-1.0, 1.0) 

    double speed = normalizeTriggerInput(ctl->throttle());  // R trigger to accelerate
    double steer = normalizeStickInput(ctl->axisX());       // L stick to steer

    if (ctl->y() || ctl->r1()) // reverse while holding Y or R1
      speed = -speed;

    // update servo position
    GroveServo.updateServo(steer);

    // update motor speed: value > 0 (forward), value < 0 (reverse), value = 0 (coast)
    MK.updateMotorSpeed(CHANNEL_A, speed);  

    // all channel outputs are combined into a single Bluetooth packet for MK module, 
    // so the the outgoing payload is refreshed once after all channels where updated
    MK.applyUpdates();

    // for more details on controller data processing see TestBluePad32 example and Bluepad32 docs
  };

  // failsafe() is called when no data is received from controller after timeout
  void failsafe() {
    GroveServo.stop();
    MK.stop();
  };

  // this will prevent running the servo from USB Power
  // isLowBattery() will return 'true' only if the HDriverBase is connected to the MK Module
  bool isLowBatteryState() {
    return HDriverBaseExt.isLowBattery();
  }

  // this method is called after 5 minutes of inactivity (no controller input and no motor output)
  void idleTimer() {
    // stop sending packets so MK module will power off after a while
    MK.disconnect();
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
