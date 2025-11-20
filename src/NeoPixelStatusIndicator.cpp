//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "NeoPixelStatusIndicator.h"
#include "BluepadHub.h"

#include <Adafruit_NeoPixel.h>

void NeoPixelStatusIndicator::begin(int16_t pin) {

  pixels = new Adafruit_NeoPixel(1, pin, NEO_GRB + NEO_KHZ800);
  pixels->begin();
  pixels->setBrightness(ledBrightness);
}

void NeoPixelStatusIndicator::setBrightness(uint8_t _ledBrightness) {
  ledBrightness = _ledBrightness;
  
  if (pixels != nullptr)
    pixels->setBrightness(ledBrightness);
}

void NeoPixelStatusIndicator::setLedColor(unsigned long color) {
  pixels->clear();
  pixels->setPixelColor(0,color);  
  pixels->show();
}

void NeoPixelStatusIndicator::showStatusPattern() {

  if (statusPattern == StatusPattern::Idle) {
    setLedColor(0x00FF00);  
    patternDelayMillis(500);
  }
  
  if (statusPattern == StatusPattern::Error) {
    setLedColor(0xFF0000);  
    vTaskDelayMillis(500);
    setLedColor(0);  
    patternDelayMillis(500);   
  }

  if (statusPattern == StatusPattern::Warning) {
    setLedColor(0xFFA500);  
    vTaskDelayMillis(500);
    setLedColor(0);  
    patternDelayMillis(500);   
  }

  if (statusPattern == StatusPattern::Pairing) {
    setLedColor(0x0000FF);  
    vTaskDelayMillis(500);
    setLedColor(0);  
    patternDelayMillis(500);   
  }

  if (statusPattern == StatusPattern::Connected) {
    setLedColor(0x0000FF);  
    patternDelayMillis(500);
  }
}

void NeoPixelStatusIndicator::showEventPattern() {

  if (eventPattern == EventPattern::Reset) {
    for (int i=0; i<5; i++) {
      setLedColor(0xFF0000);  
      vTaskDelayMillis(200);
      setLedColor(0);  
      vTaskDelayMillis(200);
    }
  }

  if (eventPattern == EventPattern::ProfileSelect) {
    setLedColor(0xFFFFFF);  
    vTaskDelayMillis(250);
    setLedColor(0);  
    vTaskDelayMillis(250);
  }
}
