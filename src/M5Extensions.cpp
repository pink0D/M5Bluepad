//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "M5Extensions.h"
#include "BluepadHub.h"

#include <esp_timer.h>

BluetoothPairingButtonHelper BluetoothPairingButton;

void BluetoothPairingButtonHelper::button_task(void *param) {
  static_cast<BluetoothPairingButtonHelper*>(param)->task();  
}

void BluetoothPairingButtonHelper::task() {

  while (1) {
    delay(100);

    // wait while button is up
    while (digitalRead(button_pin) == HIGH) {
      delay(100);
    }

    unsigned long time_click = esp_timer_get_time();

    bool short_click_handled = false;
    bool long_click_handled = false;

    // wait while button is pressed
    while (digitalRead(button_pin) == LOW) {

      delay(100);
      unsigned long time_now = esp_timer_get_time();

      if (time_now > time_click + short_click * 1000) {
        if (!short_click_handled) {
          short_click_handled = true;

          BluepadHub.enablePairing();
        }
      }

       if (time_now > time_click + long_click * 1000) {
        if (!long_click_handled) {
         long_click_handled = true;

          BluepadHub.resetPairing();
          BluepadHub.enablePairing();
        }
      }
    }
  }
}

void BluetoothPairingButtonHelper::begin(unsigned long _short_click, unsigned long _long_click, int _button_pin) {

  button_pin = _button_pin;
  short_click = _short_click;
  long_click = _long_click;

  pinMode(button_pin, INPUT);

  xTaskCreate(button_task, "button_task", 8*1024, this, 0, nullptr);
}