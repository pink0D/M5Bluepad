//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _M5_ATOMIC_MOTION_EXT_H_
#define _M5_ATOMIC_MOTION_EXT_H_

#include <M5AtomicMotion.h>

#include "MultiServoUnit.h"
#include "MultiMotorUnit.h"

class M5AtomicMotionExt : private M5AtomicMotion, public bluepadhub::MultiServoUnit<4>, public bluepadhub::MultiMotorUnit<2> {
  public:
    M5AtomicMotionExt() {};

    bool begin(TwoWire *_wire = &Wire, uint8_t addr = M5_ATOMIC_MOTION_I2C_ADDR,
      uint8_t sda = 25, uint8_t scl = 21, long freq = 100000) {
        
      return M5AtomicMotion::begin(_wire, addr, sda, scl, freq);
    };

    void stop() {
      stopMotors();
      stopServos();
    };

  private:
    virtual void outputServoPulse(int channel, uint16_t pulse);
    virtual void outputMotorSpeed(int channel, double normalized_speed);
    
};

#endif