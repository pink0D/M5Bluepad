//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericMotor.h"

void GenericMotor::setController(GenericMotorController* _controller, int _channel) {
  this->controller = _controller;
  this->channel = _channel;
}

void GenericMotor::stop() {
  updateSpeed(0);
};

void GenericMotor::setLimits(double limit_min, double limit_max) {
  ChannelFilter::setLimits(limit_min, limit_max);
}

void GenericMotor::updateSpeed(double normalized_speed) {
  controller->outputMotorSpeed(channel, updateValue(normalized_speed));
}

void GenericMotor::brake() {
  updateValue(0);
  controller->outputMotorBrake(channel);
}