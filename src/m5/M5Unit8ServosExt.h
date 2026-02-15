//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _M5_UNIT_8SERVOS_EXT_H_
#define _M5_UNIT_8SERVOS_EXT_H_

#include "MultiServoUnit.h"
#include "M5Unit8Servos.h"

class M5Unit8ServosExt : public M5Unit8Servos, public bluepadhub::MultiServoUnit<8> {
  public:
    M5Unit8ServosExt() {};

    bool begin(TwoWire *wire = &Wire1, uint8_t sda = 26, uint8_t scl = 32,
               uint8_t addr = M5_UNIT_8SERVO_DEFAULT_ADDR) {
        
      if ( M5Unit8Servos::begin(wire, sda, scl, addr) ) {

        if (setPinMode)
          setAllPinMode(SERVO_CTL_MODE);

        return true;
      }

      return false;
    };

    void stop() {
      stopServos();
    };

    void setServoMode(bool set) {
      this->setPinMode = set;
    };


  private:
    virtual void outputServoPulse(int channel, uint16_t pulse);

    bool setPinMode = true;
    
};

#endif