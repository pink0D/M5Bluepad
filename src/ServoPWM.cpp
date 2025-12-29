//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ServoPWM.h"

namespace bluepadhub {

  void ServoPWM::begin(int pwm_pin) {
    
    if (pwm_pin == 0)
    {
      Serial.print("ServoPWM::begin pin not set"); 
      return;
    }

    attach(pwm_pin, servoPulseMin, servoPulseMax);  
    writeMicroseconds(1500);
    setController(this, 0);         
  };

  void ServoPWM::outputServoPulse(int channel, uint16_t pulse) {
    if (!attached())
      return; // don't update any outputs if it was not initialized

    writeMicroseconds((int) pulse);
  }

}
