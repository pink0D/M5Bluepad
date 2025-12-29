//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _M5_ATOM_H_
#define _M5_ATOM_H_

#include "NeoPixelStatusIndicator.h"
#include "DeepSleep.h"
#include "MultiFunctionButton.h"

class M5AtomLiteIndicator : public bluepadhub::NeoPixelStatusIndicator {
  public:

    M5AtomLiteIndicator() {};

    virtual void begin(int16_t pin_rgb = 27, int16_t pin_ir = 12, bool setBluepadHubStatusIndicator = true);    
}; 
  
class M5AtomDeepSleep : public bluepadhub::DeepSleep {
    public:
        M5AtomDeepSleep() {}; 

    protected:
        virtual void afterStartup();
        virtual void beforeSleep();
};

class M5AtomLiteButton : public bluepadhub::MultiFunctionButton {
  public:
    M5AtomLiteButton() {};

    void begin(unsigned long _click_time = 500, unsigned long _hold1_time = 5000, unsigned long _hold2_time = 10000, int _button_pin = 39, int _pin_mode = INPUT_PULLUP) {
      bluepadhub::MultiFunctionButton::begin(_click_time, _hold1_time, _hold2_time, _button_pin, _pin_mode);
    };
};


#endif