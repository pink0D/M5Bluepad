//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericServo.h"

void GenericServo::setController(GenericServoController* _controller, int _channel) {
  this->controller = _controller;
  this->channel = _channel;
}

void GenericServo::stop() {
  updateServo(0);
};

void GenericServo::setServoPulseRange(uint16_t pulse_min, uint16_t pulse_max) {

  if (pulse_min < 500)
    pulse_min = 500;
  if (pulse_max > 2500)
    pulse_max = 2500;

  servoPulseMin = pulse_min;
  servoPulseMax = pulse_max;
}

void GenericServo::setServoMaxAngle(uint16_t angle_max) {

  if (angle_max > 180)
    angle_max = 180;

  double delta = 500.0 * ( ((double) angle_max) / 90.0 );

  servoPulseMin = 1500 - delta;
  servoPulseMax = 1500 + delta;
}

void GenericServo::updateServo(double normalized_position) {

  double mid   = (servoPulseMax + servoPulseMin) / 2.0;
  double width = (servoPulseMax - servoPulseMin) / 2.0;

  double pulse = mid + width * updateValue(normalized_position);

  controller->outputServoPulse(channel, pulse);
}