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

M5AtomLiteIndicator   AtomLiteIndicator;  // RGB LED status indication
M5AtomLiteButton      AtomLiteButton;     // button click handler
M5AtomicMotionExt     AtomicMotionExt;    // M5AtomicMotion extension


// this class handles controller input ans sets channel outputs
class : public bluepadhub::Profile {

  // this method is implicitly called after controller startup
  void setup() {

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

    // use Atom Lite RGB LED for status indication
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
            
    while(!AtomicMotionExt.begin()) {
      Serial.println("Atomic Motion begin failed");
      AtomLiteIndicator.setErrorStatus();
      delay(1000);
    }

    // hold button to enable pairing, continue holding to forget paired devices  
    AtomLiteButton.begin();

    // specify 500..2500us pulse for 360-servo
    // if no pulse range is specified, then default 1000..2000us will be used (180-servo)
    AtomicMotionExt.setServoPulseRange(SERVO_1, 500, 2500);
    AtomicMotionExt.setServoPulseRange(SERVO_2, 500, 2500);
    AtomicMotionExt.setServoPulseRange(SERVO_3, 500, 2500);
    AtomicMotionExt.setServoPulseRange(SERVO_4, 500, 2500);

    // alternatevely, you can specify servo rotation angle in degrees
    // this will be recalculated to corresponding pulse range internally
    // AtomicMotionExt.setServoMaxAngle(SERVO_1, 45);
    // AtomicMotionExt.setServoMaxAngle(SERVO_2, 90);

    // PWM duty cycle for M1 & M2 can be limited from 0 to 100% (default is full range)
    // usually this is not needed, but some motors start only when PWM is more than 25-30%
    // AtomicMotionExt.setMotorLimits(MOTOR_1, 0.25, 1.0);

    // if necessary, the upper PWM limit can be also specified to limit maximum motor power 
    // AtomicMotionExt.setMotorLimits(MOTOR_2, 0.25, 0.75);
  };

  // process updates from controller
  void processBluepadController(BluepadController* ctl) {

    static int motor1_direction = 1;
    static int motor2_direction = 1;

    // returns 'true' only once when button is pressed
    if (wasClicked(ctl->l1()))
      motor1_direction = -motor1_direction;

    if (wasClicked(ctl->r1()))
      motor2_direction = -motor2_direction;


    // updateServo/updateMotor methods set new values for servo/motor channels output with anti-jitter filtering
    // argument is expected to be a normalized value between -1.0 and +1.0
    // 1.0 = full speed forward / max servo rotation clockwise
    // -1.0 = full speed reverse / max servo rotation counter-clockwise
    // 0.0 stops motors / returns servo to zero position
    // 0.5 / 0.75 or any other double value = set motor speed / servo rotation to 50% / 75% / etc

    // normalizeStickInput/normalizeTriggerInput methods apply deadzone correction to raw values
    // then integer values from controller are mapped to normalized range (-1.0, 1.0) 
   
    AtomicMotionExt.updateServo(SERVO_1, normalizeStickInput(ctl->axisX()));
    AtomicMotionExt.updateServo(SERVO_2, normalizeStickInput(ctl->axisY()));
    AtomicMotionExt.updateServo(SERVO_3, normalizeStickInput(ctl->axisRX()));
    AtomicMotionExt.updateServo(SERVO_4, normalizeStickInput(ctl->axisRY()));

    AtomicMotionExt.updateMotor(MOTOR_1, motor1_direction * normalizeTriggerInput(ctl->brake()));
    AtomicMotionExt.updateMotor(MOTOR_2, motor2_direction * normalizeTriggerInput(ctl->throttle()));

    // for more details on controller data processing see TestBluePad32 example and Bluepad32 docs
  };

  // this method is called when no data is received from controller after timeout
  void failsafe() {
    AtomicMotionExt.stop();
  };

} TestProfile;
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
