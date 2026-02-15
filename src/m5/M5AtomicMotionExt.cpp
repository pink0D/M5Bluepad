//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "M5AtomicMotionExt.h"

void M5AtomicMotionExt::outputServoPulse(int channel, uint16_t pulse) {

  if (channel < 0) 
    return;

  setServoPulse(channel, pulse);
}

void M5AtomicMotionExt::outputMotorSpeed(int channel, double normalized_speed) {

  if (channel < 0) 
    return;

  setMotorSpeed(channel, normalized_speed * 127.0);
}