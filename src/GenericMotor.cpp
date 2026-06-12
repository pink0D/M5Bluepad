//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericMotor.h"

namespace bluepadhub {

  void GenericMotor::setController(GenericMotorController* _controller, int _channel) {
    this->controller = _controller;
    this->channel = _channel;
  }

  void GenericMotor::stop() {
    updateSpeed(0);
  };

  void GenericMotor::setLimits(float limit_min, float limit_max) {
    OutputFilter::setLimits(limit_min, limit_max);
  }

  void GenericMotor::updateSpeed(float normalized_speed) {
    if (controller != nullptr)
      controller->outputMotorSpeed(channel, updateValue(normalized_speed));
    else
      Serial.println("WARNING: no MotorController was set for motor");
  }

  void GenericMotor::brake() {
    updateValue(0);
    if (controller != nullptr)
      controller->outputMotorBrake(channel);
    else
      Serial.println("WARNING: no MotorController was set for motor");
  }

}