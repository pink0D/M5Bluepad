//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

//
// See https://github.com/pink0D/CameraBrick for more details on creating GeekServo adapter with ESP32CAM
//
// This sketch is an alternate way for controlling the servo with the adapter:
// 1. The gamepad is connected to ESP32
// 2. Input is processed by ESP32
// 3. ESP32 directly outputs servo signal to a PWM-pin
// 4. Power Functions motors are connected to the Mould King module, which is controlled by ESP32 via Bluetooth
//

#include <BluepadHub.h>
#include <MouldKingExtensions.h>

#include "LedIndicator.h"

// MK 4.0 module
MouldKing40Ext mk;   //MK 6.0 is also supported (see MouldKing60Ext class)
GeekServo servo(12);   // servo signal pin = 12
LedIndicator led;

// this class handles controller input ans sets channel outputs
class : public bluepadhub::Profile {

  // this method is implicitly called after controller startup
  void setup() {
  
    // use Atom Lite RGB led for status indication
    led.setBrightness(2); 
    led.begin(4);         // ESP32CAM LED pin
    

    // uncomment to automatically start pairing every time the controller is started

    // enablePairingAfterStartup = true;

    
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
    // servo.setServoPulseRange(500, 2500);  

    // limit servo rotation to 75 degrees (pulse range is calculated internally)
    servo.setServoMaxAngle(75);
    servo.begin();
  };

  // this method is called after BLE initialization is complete
  // since MK module is controlled over Bluetooth, connectAsync() method must be here, not in setup()
  void afterSetup() {
    mk.connectAsync();

    // comment this line after first pairing
    BluepadHub.enablePairing();
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
    servo.updateServo(steer);

    // update motor speed: value > 0 (forward), value < 0 (reverse), value = 0 (coast)
    mk.updateMotorSpeed(MOTOR_A, speed);  

    // all channel outputs are combined into a single Bluetooth packet for MK module, 
    // so the outgoing payload is refreshed once after all channels where updated
    mk.applyUpdates();

    // for more details on controller data processing see TestBluePad32 example and Bluepad32 docs
  };

  // failsafe() is called when no data is received from controller after timeout
  void failsafe() {
    servo.stop();
    mk.stop();
  };

  // this method is called after 5 minutes of inactivity (no controller input and no motor output)
  void idleTimer() {
    // stop sending packets so MK module will power off after a while (does not work  - MK remains powered on)
    //mk.disconnect();
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
