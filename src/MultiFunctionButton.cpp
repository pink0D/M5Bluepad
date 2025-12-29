//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MultiFunctionButton.h"
#include "BluepadHub.h"

#include <esp_timer.h>

namespace bluepadhub {

  void MultiFunctionButton::button_task(void *param) {
    static_cast<MultiFunctionButton*>(param)->task();  
  }

  void MultiFunctionButton::task() {

    vTaskDelayMillis(1000); // wait 1sec after startup

    // power button might remain pressed after startup - wait until it is released
    while (readButtonState(button_pin) == LOW) {
      vTaskDelayMillis(100);
    }

    while (1) {
      vTaskDelayMillis(100);

      // wait while button is up
      while (readButtonState(button_pin) == HIGH) {
        vTaskDelayMillis(100);
      }

      unsigned long time_click = esp_timer_get_time();

      bool click_handled = false;
      bool hold1_handled = false;
      bool hold2_handled = false;

      // wait while button is pressed
      while (readButtonState(button_pin) == LOW) {

        vTaskDelayMillis(10);
        unsigned long time_now = esp_timer_get_time();

        if (time_now > time_click + click_time * 1000) {
          if (!click_handled) {
            click_handled = true;

            ::BluepadHub.getProfile()->processButtonEvent(MultiFunctionButton::EventType::Click, 
                                                          MultiFunctionButton::ButtonState::Pressed);           
          }
        }

        if (time_now > time_click + hold1_time * 1000) {
          if (!hold1_handled) {
            hold1_handled = true;

            ::BluepadHub.getProfile()->processButtonEvent(MultiFunctionButton::EventType::Hold1, 
                                                          MultiFunctionButton::ButtonState::Pressed);           
          }
        }

        if (time_now > time_click + hold2_time * 1000) {
          if (!hold2_handled) {
            hold2_handled = true;

            ::BluepadHub.getProfile()->processButtonEvent(MultiFunctionButton::EventType::Hold2, 
                                                          MultiFunctionButton::ButtonState::Pressed);           
          }
        }
      }

      // button was pressed but released before hold1 & hold2 timeouts
      if ( click_handled && (!hold1_handled) && (!hold2_handled) ) {

        ::BluepadHub.getProfile()->processButtonEvent(MultiFunctionButton::EventType::Click, 
                                                      MultiFunctionButton::ButtonState::Released);           
      }
    }
  }

  void MultiFunctionButton::begin(unsigned long _click_time, unsigned long _hold1_time, unsigned long _hold2_time, int _button_pin, int _pin_mode) {

    button_pin = _button_pin;
    click_time = _click_time;
    hold1_time = _hold1_time;
    hold2_time = _hold2_time;

    pinMode(button_pin, _pin_mode);

    xTaskCreate(button_task, "button_task", 8*1024, this, 0, nullptr);
  }

  int MultiFunctionButton::readButtonState(int pin) {
    return digitalRead(pin);
  }

}