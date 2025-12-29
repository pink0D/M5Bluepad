//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _CONTROL_PROFILE_H_
#define _CONTROL_PROFILE_H_

#include <stddef.h>
#include <stdint.h>
#include <map>

#include <Bluepad32.h>

#include "MultiFunctionButton.h"

#define wasClicked(btn) wasClickedImpl(__LINE__,(btn))

typedef Controller BluepadController;

namespace bluepadhub {

  struct ConfigParams { 

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

    bool controllerAutoDisconnect = true;   // drop BT connection after idle timeout
    long controllerTimeout = 500;           // stop motors when no data from controller is received (possibly out of range)
    long idleTimeout       = 300000;        // power off when there's no input/output
    long deepSleepTimeout  = 30*60000;      // deep sleep timer wakeup
    long deepSleepTaskMaxDuration = 60000;  // max duration for executing task in deep sleep mode

    bool forgetBluetoothDevices = false;    // reset BT pairing on every startup
    bool enablePairingAfterStartup = false; // go to pairing mode after every startup
  };  
  
  class Profile : public ConfigParams {
    public:
      Profile(bool setBluepadHubProfile = true);

      virtual void beforeSetup() {};
      virtual void setup() {};   
      virtual void afterSetup() {};   
      virtual void processBluepadController(BluepadController *ctl) {};
      virtual void failsafe() {};
      virtual void processButtonClick() {};
      virtual void processButtonEvent(MultiFunctionButton::EventType eventType, MultiFunctionButton::ButtonState buttonState);
      virtual bool isLowBatteryState() { return false; };
      virtual bool isFaultState() { return false; };
      virtual void idleTimer() {};
      virtual void deepSleepTimer() {};

      // returns true only once if controller button was clicked
      static bool wasClickedImpl(int index, bool newValue);

      // returns normalized value in 0..1 range, with deadzones correction
      double normalizeTriggerInput(int input);

      // returns normalized value in -1..1 range, with center and outer deadzone correction
      double normalizeStickInput(int input);
  };

}

#endif
