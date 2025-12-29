//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "BluepadHub.h"

#include <esp_timer.h>

// global instance
bluepadhub::BluepadHub BluepadHub;


namespace bluepadhub {

  void BluepadHub::setProfile(Profile* _profile) {
    if (profile != nullptr) {
      Serial.println("WARNING: profile was already set");
    }
    if (_profile != nullptr) {
      profile = _profile;
      Serial.println("BluepadHub profile updated");
    }
  }

  void BluepadHub::setDeepSleep(DeepSleep* _deepSleep) {
    deepSleep = _deepSleep;
    if (_deepSleep != nullptr) {
      Serial.println("BluepadHub deep sleep updated");
    }
  }
  
  void BluepadHub::begin() {

    Serial.begin(115200);

    if (profile == nullptr) {
      Serial.println("WARNING: Profile was not set");
      profile = new Profile(false);
    }

    profile->beforeSetup();

    profile->setup();

    if (statusIndicator == nullptr) {
      Serial.println("WARNING: StatusIndicator was not set");
      statusIndicator = new StatusIndicator();
    }

    time_idle_wait = profile->idleTimeout * 1000;
   
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);

    auto initialStatusPattern = StatusIndicator::StatusPattern::Idle;

    if (profile->forgetBluetoothDevices) {
      BP32.forgetBluetoothKeys();
      BP32.enableNewBluetoothConnections(true);
      initialStatusPattern = StatusIndicator::StatusPattern::Pairing;
    } else {
      BP32.enableNewBluetoothConnections(false); // pairing disabled by default
    }

    statusIndicator->setStatusPattern(initialStatusPattern);

    Serial.println("BluepadHub::begin completed");

    profile->afterSetup();

    if (profile->enablePairingAfterStartup) {
      enablePairing();
    }
  };

  void BluepadHub::update() {
    vTaskDelay(1);

    // always update Bluepad32 data
    bool controllerUpdated = BP32.update();

    long time_now = esp_timer_get_time();

    // stop motors if controller is not sending updates (possible out of range)
    if ( (time_controller_update > 0) && (time_now > time_controller_update + profile->controllerTimeout*1000) ){
      Serial.println("Controller timeout");
      time_controller_update = 0;
      
      profile->failsafe();
    }

    // idle timeout 
    static bool idle_timeout_handled = false;

    if (time_now > time_idle_wait) {

      if (!idle_timeout_handled) {

        Serial.println("Idle timeout");

        if (profile->controllerAutoDisconnect)
          disconnectController();

        profile->idleTimer();
         
        idle_timeout_handled = true;
      }

    }

    // check fault state
    if (profile->isFaultState()) {  
         
      statusIndicator->setErrorStatus();  
    } else {
      statusIndicator->clearErrorStatus();
    }

    // check low battery state
    if (profile->isLowBatteryState()) {
      
      profile->failsafe();
      statusIndicator->setWarningStatus();  
      OutputFilter::resetIdleState(); // reset idle state so timeout can be triggered later
      
      return; // no controller processing in low battery mode

    } else {
      statusIndicator->clearWarningStatus();
    }

    // process controller input and update output
    if (controllerUpdated)
    {
      if (bp32Controller && bp32Controller->isConnected() && bp32Controller->hasData()) {
        if (bp32Controller->isGamepad()) {

          OutputFilter::resetIdleState();

          profile->processBluepadController(bp32Controller); 

          time_controller_update = time_now; 

          // reset idle timeout if any outputs where updated
          if (!OutputFilter::isIdleState()) {
            time_idle_wait = time_now + profile->idleTimeout * 1000;
            idle_timeout_handled = false;
          }

        }
        else {
          Serial.println("BP32: disconecting controller since it is not a gamepad");
          disconnectController();
        }
      }
    } else {
      // vTaskDelay(100);
    }

  }

  void BluepadHub::onConnectedController(BluepadController* ctl) {
    if (::BluepadHub.bp32Controller == nullptr) {
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("BP32: controller connected: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      ::BluepadHub.bp32Controller = ctl;
      ::BluepadHub.time_controller_update = 0;
      BP32.enableNewBluetoothConnections(false); // exit pairing mode after connection
      
      ::BluepadHub.statusIndicator->setStatusPattern(StatusIndicator::StatusPattern::Connected);

    } else {
      Serial.println("BP32: refusing new connection since another controller is already connected");
      ctl->disconnect();
    }
  }

  void BluepadHub::onDisconnectedController(BluepadController* ctl) {
      if (::BluepadHub.bp32Controller == ctl) {
          Serial.println("BP32: Controller disconnected");
          ::BluepadHub.bp32Controller = nullptr;
      } else {
          Serial.println("BP32: Unknown controller disconnected");
      }

      ::BluepadHub.statusIndicator->setStatusPattern(StatusIndicator::StatusPattern::Idle);
  }

  void BluepadHub::disconnectController() {
    if (bp32Controller != nullptr) {
      bp32Controller->disconnect();
      bp32Controller = nullptr;
      Serial.println("BP32: Controller disconnected");

      statusIndicator->setStatusPattern(StatusIndicator::StatusPattern::Idle);

      vTaskDelayMillis(100); // delay to ensure BT communication is completed before power off
    }
  }

  void BluepadHub::enablePairing() {
    Serial.println("BluepadHub::enablePairing");

    BP32.enableNewBluetoothConnections(true);
    
    statusIndicator->setStatusPattern(StatusIndicator::StatusPattern::Pairing);
  }

  void BluepadHub::resetPairing() {
    Serial.println("BluepadHub::resetPairing");

    disconnectController();
    BP32.forgetBluetoothKeys();

    statusIndicator->setEventPattern(StatusIndicator::EventPattern::Reset);
  }

  void BluepadHub::startDeepSleep() {

    if (isDeepSleepEnabled()) {
      statusIndicator->stop();
      disconnectController();      
      deepSleep->startDeepSleep();    
    } else {
      Serial.println("WARNING: deep sleep was not initialized");
    }
  }  
}

      