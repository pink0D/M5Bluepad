//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _BLUEPAD_HUB_H_
#define _BLUEPAD_HUB_H_

#define TASK_STACK_SIZE 8*1024
#define TASK_DEFAULT_DELAY_MILLIS 50

#define vTaskDelayMillis(m) vTaskDelay(pdMS_TO_TICKS(m));

#include <Arduino.h>

#include <Bluepad32.h>

#include "ConfigProfile.h"
#include "StatusIndicator.h"
#include "OutputFilter.h"
#include "DeepSleep.h"
#include "ServoPWM.h"

namespace bluepadhub {

  class BluepadHub {
    public:

      BluepadHub() {};

      void setStatusIndicator(StatusIndicator* _statusIndicator) {
        statusIndicator = _statusIndicator;
      };

      StatusIndicator* getStatusIndicator() {
        return statusIndicator;
      };

      void setProfile(Profile* _profile);

      Profile* getProfile() {
        return profile;
      };

      void setDeepSleep(DeepSleep* _deepSleep);

      bool isDeepSleepEnabled() {
        return deepSleep != nullptr;
      }

      void begin();
      void update();

      void enablePairing();
      void resetPairing();
      void disconnectController();  
      
      void startDeepSleep();

    private:

      void onConnectedController(BluepadController* ctl);
      void onDisconnectedController(BluepadController* ctl);

      BluepadController* bp32Controller = nullptr;

      long time_controller_update = 0;
      long time_idle_wait = 0;
      bool idle_timeout_handled = false;

      Profile* profile = nullptr;
      StatusIndicator* statusIndicator = nullptr;
      DeepSleep* deepSleep = nullptr;
  };
}

extern bluepadhub::BluepadHub BluepadHub;  

#endif