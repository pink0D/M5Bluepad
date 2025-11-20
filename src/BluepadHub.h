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

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>
#include <map>

#include <Bluepad32.h>

#define TASK_STACK_SIZE 8*1024
#define TASK_DEFAULT_DELAY_MILLIS 50

#define vTaskDelayMillis(m) vTaskDelay(pdMS_TO_TICKS(m));

#define wasClicked(btn) wasClickedImpl(__LINE__,(btn))

namespace bluepadhub {

  class ControlProfile; // forward declaration

  typedef std::function<bool()> CheckFunctionPtr;

  struct BluepadHubConfig { 

    /*
    Controller input adjustments
    */

    int controllerStickDeadzoneLow = 50; 
    int controllerStickDeadzoneHigh = 500;

    int controllerTriggerDeadzoneLow = 5;
    int controllerTriggerDeadzoneHigh = 1000;

    /*
    Timeouts (milliseconds)
    */

    long controllerTimeout = 500;    // stop motors when no data from controller is received (possibly out of range)
    bool controllerAutoDisconnect = false; // drop BT connection if no controller data received
    bool forgetBluetoothDevices = false;
  };

  class StatusIndicator {
    public:
      StatusIndicator() {};

      enum class StatusPattern {None, Idle, Pairing, Connected, Error, Warning};
      enum class EventPattern {None, PowerOff, Reset, ProfileSelect, SettingSaved};

      virtual void begin() {};

      void setStatusPattern(StatusPattern statusPattern) {
        
        if (this->statusPattern != statusPattern) {
          statusPatternPrevious = this->statusPattern;
        }

        this->statusPattern = statusPattern;
      };

      void restorePreviousStatusPattern() {
        if (statusPatternPrevious != StatusPattern::None) {
          this->statusPattern = statusPatternPrevious;
        }
      }

      void setErrorStatus() {
        setStatusPattern(StatusPattern::Error);
      };

      void clearErrorStatus() {
        if (statusPattern == StatusPattern::Error) {
          restorePreviousStatusPattern();
        }
      };

      void setWarningStatus() {
        setStatusPattern(StatusPattern::Warning);
      };

      void clearWarningStatus() {
        if (statusPattern == StatusPattern::Warning) {
          restorePreviousStatusPattern();
        }
      };

      void setEventPattern(EventPattern eventPattern) {
        this->eventPattern = eventPattern;
      };  

      virtual void showStatusPattern() { vTaskDelay(100); };
      virtual void showEventPattern() { vTaskDelay(100); };

    protected:
      StatusPattern statusPattern = StatusPattern::None;
      StatusPattern statusPatternPrevious = StatusPattern::None;
      EventPattern eventPattern = EventPattern::None;

      void patternDelayMillis(int timeout);

  };  

  class BluepadHub {
    public:

      BluepadHub() {};

      inline BluepadHubConfig* config() {
        return &cfg;
      }

      void setStatusIndicator(StatusIndicator* _indicator) {
        indicator = _indicator;
      }

      void setControlProfile(ControlProfile* _profile);

      void begin(bool forgetBluetoothDevices = false) {
        auto cfg = BluepadHubConfig();
        cfg.forgetBluetoothDevices = forgetBluetoothDevices;
        begin(cfg);
      }

      void begin(BluepadHubConfig config);
      void update();

      void enablePairing();
      void resetPairing();
      void disconnectController();

      void setLowBatteryCheck(const CheckFunctionPtr &isLowBatteryFPtr) {
        this->isLowBatteryFPtr = isLowBatteryFPtr;
      };

      void setFaultCheck(const CheckFunctionPtr &isFaultFPtr) {
        this->isFaultFPtr = isFaultFPtr;
      };

    private:

      BluepadHubConfig cfg;

      static void onConnectedController(ControllerPtr ctl);
      static void onDisconnectedController(ControllerPtr ctl);

      ControllerPtr bp32Controller = nullptr;

      long time_controller_update = 0;

      ControlProfile *profile = nullptr;
      StatusIndicator *indicator = nullptr;

      static void taskStatusIndicator(void *param);

      CheckFunctionPtr isLowBatteryFPtr = nullptr;
      CheckFunctionPtr isFaultFPtr = nullptr;
  };
}

extern bluepadhub::BluepadHub BluepadHub;  

namespace bluepadhub {
  
  class ControlProfile {
    public:
      ControlProfile() {};

      virtual void begin() {};
      virtual void update(ControllerPtr ctl) {};
      virtual void failsafe() {};

      // returns true only once if controller button was clicked
      static inline bool wasClickedImpl(int index, bool newValue) {

        static std::map<int, bool> currentClickValues;
        
        if (currentClickValues.count(index) == 0)
          currentClickValues[index] = false;

        bool click = false;
        if (newValue) {
          click = !(currentClickValues[index]);
        }
        currentClickValues[index] = newValue;
        return click;
      };

      // returns normalized value in 0..1 range, with deadzones correction
      static inline double normalizeTriggerInput(int input) {

        if (input < ::BluepadHub.config()->controllerTriggerDeadzoneLow)
          return 0;

        if (input > ::BluepadHub.config()->controllerTriggerDeadzoneHigh)
          return 1;

        return ((double)input) / 1024.0 ;
      };

      // returns normalized value in -1..1 range, with center and outer deadzone correction
      static inline double normalizeStickInput(int input) {

        if (abs(input) < ::BluepadHub.config()->controllerStickDeadzoneLow)
          return 0;

        double sign = input > 0 ? 1.0 : -1.0;
        if (abs(input) > ::BluepadHub.config()->controllerStickDeadzoneHigh)
          return sign;

        return ((double)input) / 512.0;
      };

  };

}

#endif