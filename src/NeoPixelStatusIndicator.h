//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "BluepadHub.h"

namespace bluepadhub {

  class NeoPixelStatusIndicator : public StatusIndicator {
    public:

      NeoPixelStatusIndicator() {};

      virtual void begin(int16_t pin, bool setBluepadHubStatusIndicator = true);
      virtual void clear();

      virtual void showStatusPattern();
      virtual void showEventPattern();

      void setBrightness(uint8_t _ledBrightness);

    private:
      Adafruit_NeoPixel *pixels = nullptr;
      void setLedColor(unsigned long color);

      uint8_t ledBrightness = 255;
  };  
  
}
