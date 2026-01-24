//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "StatusIndicator.h"

#include "BluepadHub.h"

namespace bluepadhub {

  void StatusIndicator::task() {
    
    while (1) {
      vTaskDelay(1);
      showStatusPattern();
    }
  }

  void StatusIndicator::begin(bool setBluepadHubStatusIndicator) {

    if (setBluepadHubStatusIndicator) {
        ::BluepadHub.setStatusIndicator(this);
    }
    
    xTaskCreate( [](void* ctx) {

        static_cast<StatusIndicator*>(ctx)->task();

    }, "StatusIndicator", TASK_STACK_SIZE, this, 0, NULL);
  }

  void StatusIndicator::patternDelayMillis(int timeout) {
    int time_to_wait = timeout - TASK_DEFAULT_DELAY_MILLIS;
    while (time_to_wait > 0){
      vTaskDelayMillis(TASK_DEFAULT_DELAY_MILLIS);
      time_to_wait -= TASK_DEFAULT_DELAY_MILLIS;

      if (eventPattern != EventPattern::None) {

        showEventPattern();
        eventPattern = EventPattern::None;
      }
    }
  }    

  void StatusIndicator::showStatusPattern() {
     vTaskDelay(100); 
  }

  void StatusIndicator::showEventPattern() { 
    vTaskDelay(100); 
  }

}