//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "M5Unit8ServosExt.h"

void M5Unit8ServosExt::outputServoPulse(int channel, uint16_t pulse) {

  if (channel < 0) 
    return;

  setServoPulse(channel, pulse);
}
