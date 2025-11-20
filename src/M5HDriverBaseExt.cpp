//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "M5HDriverBaseExt.h"

void M5HDriverBaseExt::begin(int pwm_frequency) {

  pinMode(22, INPUT_PULLUP);
  pinMode(33, INPUT);

  MotorHBridgeDriver::begin(19, 23, pwm_frequency);
}

double M5HDriverBaseExt::readVoltage() {
    
  fault = (digitalRead(22) == 0);

  return analogReadMilliVolts(33) * 0.01;
}