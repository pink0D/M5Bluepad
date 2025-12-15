//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "ConfigProfile.h"
#include "BluepadHub.h"

namespace bluepadhub {

  Profile::Profile(bool setBluepadHubProfile) {

    if (setBluepadHubProfile) {
      ::BluepadHub.setProfile(this);
    }
  }

  bool Profile::wasClickedImpl(int index, bool newValue) {

    static std::map<int, bool> currentClickValues;
    
    if (currentClickValues.count(index) == 0)
      currentClickValues[index] = false;

    bool click = false;
    if (newValue) {
      click = !(currentClickValues[index]);
    }
    currentClickValues[index] = newValue;
    return click;
  }


  double Profile::normalizeTriggerInput(int input) {

    if (input < controllerTriggerDeadzoneLow)
      return 0;

    if (input > controllerTriggerDeadzoneHigh)
      return 1;

    return ((double)input) / 1024.0 ;
  }


  double Profile::normalizeStickInput(int input) {

    if (abs(input) < controllerStickDeadzoneLow)
      return 0;

    double sign = input > 0 ? 1.0 : -1.0;
    if (abs(input) > controllerStickDeadzoneHigh)
      return sign;

    return ((double)input) / 512.0;
  }

  void Profile::processButtonEvent(MultiFunctionButton::EventType eventType, MultiFunctionButton::ButtonState buttonState) {

    // click is processed here when Deep Sleep is configured
    if (::BluepadHub.isDeepSleepEnabled() ) {

      // turn off the LED to indicate that the click is being processed 
      if ( (eventType == MultiFunctionButton::EventType::Click) 
            && (buttonState == MultiFunctionButton::ButtonState::Pressed) ) {

        ::BluepadHub.getStatusIndicator()->setPowerOffStatus();
      } 

      // actual power off after the button is released
      if ( (eventType == MultiFunctionButton::EventType::Click) 
            && (buttonState == MultiFunctionButton::ButtonState::Released) ) {

        ::BluepadHub.startDeepSleep();
      } 
    } else { // otherwise when no Deep Sleep is configured click processing can be overriden
        if ( (eventType == MultiFunctionButton::EventType::Click) 
            && (buttonState == MultiFunctionButton::ButtonState::Released) ) {
              processButtonClick();
        }
    }

    // bluetooth pairing
    if ( (eventType == MultiFunctionButton::EventType::Hold1) 
          && (buttonState == MultiFunctionButton::ButtonState::Pressed) ) {
      ::BluepadHub.enablePairing();
    }

    if ( (eventType == MultiFunctionButton::EventType::Hold2) 
          && (buttonState == MultiFunctionButton::ButtonState::Pressed) ) {
      ::BluepadHub.resetPairing();
      ::BluepadHub.enablePairing();
    }
  }

}
