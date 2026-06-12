//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericServo.h"

namespace bluepadhub {

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
    servoPulseMiddle = (servoPulseMax + servoPulseMin) / 2.0;
    servoPulseWidth = (servoPulseMax - servoPulseMin) / 2.0;
  }

  void GenericServo::setServoMaxAngle(uint16_t angle_max) {

    if (angle_max > 180)
      angle_max = 180;

    float delta = 500.0 * ( ((float) angle_max) / 90.0 );

    servoPulseMin = 1500 - delta;
    servoPulseMax = 1500 + delta;
    servoPulseMiddle = 1500;
    servoPulseWidth = delta;
  }

  void GenericServo::updateServo(float normalized_position) {

    float pulse = servoPulseMiddle + servoPulseWidth * updateValue(normalized_position);

    if (controller != nullptr)
      controller->outputServoPulse(channel, pulse);
    else
      Serial.println("WARNING: no ServoController was set for servo");
  }

}