//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "M5HDriverBaseExt.h"

#include <driver/rtc_io.h>
#include <driver/gpio.h>

void M5HDriverBaseExt::begin(int pwm_frequency) {

  setupPins();

  MotorDriverHBridge::begin(19, 23, pwm_frequency);
}

void M5HDriverBaseExt::setupPins() {
  
  static bool setupDone = false;

  if (!setupDone) {
    
    rtc_gpio_hold_dis(GPIO_NUM_33); // needed if this call is made after deep sleep

    pinMode(22, INPUT_PULLUP);
    pinMode(33, INPUT);

    setupDone = true;
  }
}

double M5HDriverBaseExt::readVoltage() {
    
  setupPins();
  
  return analogReadMilliVolts(33) * 0.01;
}

void M5HDriverBaseExt::updateStatus() {

  fault = (digitalRead(22) == 0);
}