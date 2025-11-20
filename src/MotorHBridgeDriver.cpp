//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MotorHBridgeDriver.h"

void MotorHBridgeDriver::begin(int pwm_pin1, int pwm_pin2, int pwm_frequency) {
  pwm_c1.attachPin(pwm_pin1, pwm_frequency);
  pwm_c2.attachPin(pwm_pin2, pwm_frequency);
  pwm_c1.write(0);
  pwm_c2.write(0);

  setController(this, 0); 
}

void MotorHBridgeDriver::outputMotorSpeed(int channel, double normalized_speed) {
  if (isZeroValue(normalized_speed)) {
    pwm_c1.write(0);
    pwm_c2.write(0);
  }
  else if (normalized_speed > 0) {
    pwm_c1.writeScaled(normalized_speed);
    pwm_c2.write(0);
  }
  else if (normalized_speed < 0) {
    pwm_c1.write(0);
    pwm_c2.writeScaled(-normalized_speed);
  }
}

void MotorHBridgeDriver::outputMotorBrake(int channel) {
  pwm_c1.writeScaled(1.0);
  pwm_c2.writeScaled(1.0);
}