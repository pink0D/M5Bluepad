//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _M5_UNIT_8SERVOS_LIGHT_CONTROLLER_H_
#define _M5_UNIT_8SERVOS_LIGHT_CONTROLLER_H_

#include "light/LightController.h"
#include "M5Unit8Servos.h"

class M5Unit8ServosLightContoller : public bluepadhub::LightController {

  private:

    bool channelActive[8];
    unsigned long channelColor[8];
    unsigned long newColor[8];

    M5Unit8Servos *unit = nullptr;


  public:

    M5Unit8ServosLightContoller() {

        for (int channel=0; channel<8; channel++) {
            channelActive[channel] = false;
            channelColor[channel] = 0;
            newColor[channel] = 0;
      }
    };

    M5Unit8ServosLightContoller(M5Unit8Servos &unit) : M5Unit8ServosLightContoller() {
        setUnit(unit);
    };

    void setUnit(M5Unit8Servos &unit) {
        this->unit = &unit;
    };

    void activateChannel(int channel) {

        if (unit == nullptr) {
            Serial.println("WARNING: no Unit 8Servos was set");
            return;
        }
        
        channelActive[channel] = true;

        unit->setOnePinMode(channel, RGB_LED_MODE);
        
        // Unit 8Servo bug: color is not set after first write, so write 2x times initially
        unit->setLEDColor(channel, 0);
        unit->setLEDColor(channel, 0); 
    };

    void resetColors() {
      for (int channel=0; channel<8; channel++) {
        newColor[channel] = 0;
      }
    };

    void setColor(int channel, unsigned long color) {
      newColor[channel] = color;
    };

    void updateColors() {
        
      if (unit == nullptr)
        return;

      for (int channel=0; channel<8; channel++) {

        if ( (channelActive[channel]) && (channelColor[channel] != newColor[channel]) ) {
          channelColor[channel] = newColor[channel];
          unit->setLEDColor(channel, channelColor[channel]);
        }
      }
    };
};

#endif