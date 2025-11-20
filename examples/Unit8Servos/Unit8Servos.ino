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

NeoPixelStatusIndicator LedRGB;     // RGB Led in Atom Lite
M5AtomicMotionExt AtomicMotionExt;  // M5AtomicMotion extension
M5Unit8ServosExt Unit8ServosExt;    // Unit8Servos extension


// this class handles controller input ans sets channel outputs

class : public bluepadhub::ControlProfile {

  // set channel parameters here
  void begin() {

    // by default, M5Unit8ServosExt.begin() sets all pins to servo mode
    // it's still posible to change mode with setOnePinMode() and use pin as Input/Output/ADC/NeoPixel/PWM

    // specify 500..2500us pulse for 360-servo
    // if no pulse range is specified, then default 1000..2000us will be used (180-servo)
    for (int channel=0; channel<8; channel++)
      Unit8ServosExt.setServoPulseRange(channel, 500, 2500);

    for (int channel=0; channel<4; channel++)
      AtomicMotionExt.setServoPulseRange(channel, 500, 2500);

    // alternatevely, you can specify servo rotation angle in degrees
    // this will be recalculated to corresponding pulse range internally
    // Unit8ServosExt.setServoMaxAngle(CHANNEL_SERVO_1, 45);
    // Unit8ServosExt.setServoMaxAngle(CHANNEL_SERVO_2, 90);
  };

  // process updates from controller
  void update(ControllerPtr ctl) {

    static int motor1_direction = 1;
    static int motor2_direction = 1;

    // returns 'true' only once when button is pressed
    if (wasClicked(ctl->l1()))
      motor1_direction = -motor1_direction;

    if (wasClicked(ctl->r1()))
      motor2_direction = -motor2_direction;

    static int active_channel_atomic_motion = 0;
    static int active_channel_unit_8servos = 0;

    // press sticks to select servo channels
    if (wasClicked(ctl->thumbL())) {
      active_channel_atomic_motion = (active_channel_atomic_motion+1) % 4;
      LedRGB.setEventPattern(bluepadhub::StatusIndicator::EventPattern::ProfileSelect);
    }

    if (wasClicked(ctl->thumbR())) {
      active_channel_unit_8servos = (active_channel_unit_8servos+1) % 8;
      LedRGB.setEventPattern(bluepadhub::StatusIndicator::EventPattern::ProfileSelect);
    }

    // updateServo/updateMotor methods set new values for servo/motor channels output with anti-jitter filtering
    // argument is expected to be a normalized value between -1.0 and +1.0
    // 1.0 = full speed forward / max servo rotation clockwise
    // -1.0 = full speed reverse / max servo rotation counter-clockwise
    // 0.0 stops motors / returns servo to zero position
    // 0.5 / 0.75 or any other double value = set motor speed / servo rotation to 50% / 75% / etc

    // normalizeStickInput/normalizeTriggerInput methods apply deadzone correction to raw values
    // then integer values from controller are mapped to normalized range (-1.0, 1.0) 
   
    // all channels in Atomic Motion Base and Unit 8Servos can be controlled simultaneously and independently
    // this example controls just selected channels with left and right stick
    Unit8ServosExt.updateServo(active_channel_unit_8servos, normalizeStickInput(ctl->axisRX()));
    
    AtomicMotionExt.updateServo(active_channel_atomic_motion, normalizeStickInput(ctl->axisX()));

    AtomicMotionExt.updateMotor(CHANNEL_MOTOR_1, motor1_direction * normalizeTriggerInput(ctl->brake()));
    AtomicMotionExt.updateMotor(CHANNEL_MOTOR_2, motor2_direction * normalizeTriggerInput(ctl->throttle()));

    // for more details on controller data processing see TestBluePad32 example and Bluepad32 docs
  };

  // this method is called when no data is received from controller after timeout
  void failsafe() {
    AtomicMotionExt.stop();
    Unit8ServosExt.stop();
  };

} TestProfile;

//  Arduino setup function
void setup() {
  
  Serial.begin(115200);

  LedRGB.setBrightness(20);
  LedRGB.begin();

  BluepadHub.setStatusIndicator(&LedRGB);
  BluepadHub.setControlProfile(&TestProfile);
  BluepadHub.begin();
  
  while(!AtomicMotionExt.begin()) {
     Serial.println("Atomic Motion begin failed");
     LedRGB.setErrorStatus();
     delay(1000);
  }

  while(!Unit8ServosExt.begin()) {
     Serial.println("Unit 8Servos begin failed");
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
