//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

//
// Instructions for using M5Bluepad
// https://github.com/pink0D/M5Bluepad
//

#include <BluepadHub.h>
#include <M5Extensions.h>

NeoPixelStatusIndicator LedRGB;     // RGB Led in Atom Lite
M5AtomicMotionExt AtomicMotionExt;  // M5AtomicMotion extension

class : public bluepadhub::ControlProfile {

  uint8_t FWD_motor = CHANNEL_SERVO_1;
  uint8_t RWD_motor = CHANNEL_SERVO_2;
  uint8_t FWD_steer = CHANNEL_SERVO_3;
  uint8_t RWD_steer = CHANNEL_SERVO_4;

  double add_steer_start = 0.75;  // rear steering start threshold

  void begin() {

    AtomicMotionExt.setServoPulseRange(FWD_motor, 500, 2500);
    AtomicMotionExt.setServoPulseRange(RWD_motor, 500, 2500);

    AtomicMotionExt.setServoMaxAngle(FWD_steer, 60);
    AtomicMotionExt.setServoMaxAngle(RWD_steer, 10);
  };

  void update(ControllerPtr ctl) {

    // default controls
    double accelerator = normalizeTriggerInput(ctl->throttle());  // R trigger to accelerate
    double steer = normalizeStickInput(ctl->axisX());             // L stick to steer

    if (ctl->y() || ctl->r1()) // reverse while holding Y or R1
      accelerator = -accelerator;

    // toggle easy mode 
    static bool easyMode = true;
    if (wasClicked(ctl->l1()))
      easyMode = !easyMode;

    if (easyMode) {
      steer = normalizeStickInput(ctl->axisRX());       // R stick to steer
      accelerator = -normalizeStickInput(ctl->axisY()); // FORWARD when L stick is UP (Y < 0)
    }
    
    double steer1 = 0;
    double steer2 = 0;

    // front axle steering is set directly from input
    steer1 = steer; 

    // rear steering is added only when input is greater than threshold 
    if (steer > add_steer_start)
      steer2 = (steer - add_steer_start) / (1.0 - add_steer_start);
    if (steer < -add_steer_start) 
      steer2 = (steer + add_steer_start) / (1.0 - add_steer_start);

    AtomicMotionExt.updateServo(FWD_motor, accelerator);
    AtomicMotionExt.updateServo(RWD_motor, accelerator);
    AtomicMotionExt.updateServo(FWD_steer, -steer1); // front and rear steering have opposite directions
    AtomicMotionExt.updateServo(RWD_steer, steer2);
  };

  void failsafe() {
    AtomicMotionExt.stop();
  };

} MonsterProfile;

void setup() {
  
  Serial.begin(115200);
  
  LedRGB.setBrightness(20);
  LedRGB.begin();

  BluepadHub.setStatusIndicator(&LedRGB);
  BluepadHub.setControlProfile(&MonsterProfile);
  BluepadHub.begin(); 

  while(!AtomicMotionExt.begin()) {
     Serial.println("Atomic Motion begin failed");
     LedRGB.setErrorStatus();
     delay(1000);
  }

  // hold Atom button for 2.5secs to enable pairing, 5secs to forget paired devices
  BluetoothPairingButton.begin(2500, 5000);

  Serial.println("Setup finished");
}

// Arduino loop function
void loop() {

  // handle inputs and update outputs
  BluepadHub.update();

  //no delay here because it's inside BluepadHub.update()
}
