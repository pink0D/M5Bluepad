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

NeoPixelStatusIndicator LedRGB;                // RGB Led in Atom Lite

M5HDriverBaseExt HDriverBaseExt;               // HDriverBase extension
ServoPWM GroveServo(SERVO_ATOM_LITE_PORT_A);   // Servo connected directy to Atom's grove port


// this class handles controller input ans sets channel outputs
class : public bluepadhub::ControlProfile {

  // set channel parameters here
  void begin() {

    // some DC motors start running only with 25% duty cycle.
    // max duty can be also specified to limit motor power
    HDriverBaseExt.setLimits(0.25, 1.0);

    // specify pulse range for a 360-servo
    // without this call, the default range (1000..2000) will be used for a 180-servo
    //
    // GroveServo.setServoPulseRange(500, 2500);

    // limit servo rotation to 75 degrees (pulse range is calculated internally)
    GroveServo.setServoMaxAngle(75);
  };

  // process updates from controller
  void update(ControllerPtr ctl) {

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

} TestProfile;

//
// WARNING
//
// HDriver can only monitor battery voltage
// When battery is low, failsafe() is called
//
// CONTROLLER IS STILL POWERED AND IDLE CURRENT CAN DRAIN AND DAMAGE THE BATTERY
// ALWAYS DISCONNECT THE BATTERY WHEN CONTROLLER IS NOT USED
//

bool battery_check() {
  return HDriverBaseExt.isLowBattery();
}

// HDriver has fault state indication for overcurrent / overload / over temperature
bool fault_check() {
  return HDriverBaseExt.isFault();
}

//  Arduino setup function
void setup() {
  
  Serial.begin(115200);

  HDriverBaseExt.begin(); 
  GroveServo.begin();

  // uncomment to enable debug output for voltage monitor  
  // HDriverBaseExt.printVoltage(true);
  
  LedRGB.setBrightness(20);
  LedRGB.begin();

  BluepadHub.setStatusIndicator(&LedRGB);
  BluepadHub.setControlProfile(&TestProfile);
  BluepadHub.setFaultCheck(&fault_check);
  BluepadHub.setLowBatteryCheck(&battery_check);

  BluepadHub.begin();

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
