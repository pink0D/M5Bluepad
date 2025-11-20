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

  void BluepadHub::begin(BluepadHubConfig config) {

    this->cfg = config;
    
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);

    auto initialStatusPattern = StatusIndicator::StatusPattern::Idle;

    if (config.forgetBluetoothDevices) {
      BP32.forgetBluetoothKeys();
      BP32.enableNewBluetoothConnections(true);
      initialStatusPattern = StatusIndicator::StatusPattern::Pairing;
    } else {
      BP32.enableNewBluetoothConnections(false); // pairing disabled by default
    }

    if (indicator != nullptr)
      indicator->setStatusPattern(initialStatusPattern);

    xTaskCreate(taskStatusIndicator, "StatusIndicator", TASK_STACK_SIZE, NULL, 0, NULL);
  };

  void BluepadHub::update() {
    vTaskDelay(1);

    long time_now = esp_timer_get_time();

    // stop motors if controller is not sending updates (possible out of range)
    if ( (time_controller_update > 0) && (time_now > time_controller_update + cfg.controllerTimeout*1000) ){
      Serial.println("Controller timeout");
      time_controller_update = 0;
      
      if (profile != nullptr)
        profile->failsafe();

      if (cfg.controllerAutoDisconnect) {
        disconnectController();

        //if (indicator != nullptr)
        //  indicator->setStatusPattern(StatusIndicator::StatusPattern::Idle);
      }
    }

    // check fault condition
    if (isFaultFPtr) {
      if (isFaultFPtr()) {
        
        if (indicator != nullptr)
          indicator->setErrorStatus();  

      } else {
        if (indicator != nullptr)
          indicator->clearErrorStatus();
      }
    }

    // check battery condition
    if (isLowBatteryFPtr) {
      if (isLowBatteryFPtr()) {
        
        if (profile != nullptr)
          profile->failsafe();

          if (indicator != nullptr)
          indicator->setWarningStatus();  
          
          return; // no controls in low battery mode

      } else {
        if (indicator != nullptr)
          indicator->clearWarningStatus();
      }
    }

    bool controllerUpdated = BP32.update();
    if (controllerUpdated)
    {
      if (bp32Controller && bp32Controller->isConnected() && bp32Controller->hasData()) {
        if (bp32Controller->isGamepad()) {

          //if (indicator != nullptr)
          //  indicator->setStatusPattern(StatusIndicator::StatusPattern::Connected);  

          if (profile != nullptr)
            profile->update(bp32Controller); 

          time_controller_update = time_now; 

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

  void BluepadHub::onConnectedController(ControllerPtr ctl) {
    if (::BluepadHub.bp32Controller == nullptr) {
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("BP32: controller connected: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      ::BluepadHub.bp32Controller = ctl;
      ::BluepadHub.time_controller_update = 0;
      BP32.enableNewBluetoothConnections(false); // exit pairing mode after connection
      
      if (::BluepadHub.indicator != nullptr)
        ::BluepadHub.indicator->setStatusPattern(StatusIndicator::StatusPattern::Connected);

    } else {
      Serial.println("BP32: refusing new connection since another controller is already connected");
      ctl->disconnect();
    }
  }

  void BluepadHub::onDisconnectedController(ControllerPtr ctl) {
      if (::BluepadHub.bp32Controller == ctl) {
          Serial.println("BP32: Controller disconnected");
          ::BluepadHub.bp32Controller = nullptr;
      } else {
          Serial.println("BP32: Unknown controller disconnected");
      }

      if (::BluepadHub.indicator != nullptr)
        ::BluepadHub.indicator->setStatusPattern(StatusIndicator::StatusPattern::Idle);
  }

  void BluepadHub::disconnectController() {
    if (bp32Controller != nullptr) {
      bp32Controller->disconnect();
      bp32Controller = nullptr;
      Serial.println("BP32: Controller disconnected");

      if (indicator != nullptr)
        indicator->setStatusPattern(StatusIndicator::StatusPattern::Idle);
    }
  }

  void BluepadHub::enablePairing() {
    Serial.println("BLUETOOTH PAIRING");

    BP32.enableNewBluetoothConnections(true);
    
    if (indicator != nullptr)
      indicator->setStatusPattern(StatusIndicator::StatusPattern::Pairing);
  }

  void BluepadHub::resetPairing() {
    Serial.println("BLUETOOTH RESET");

    disconnectController();
    BP32.forgetBluetoothKeys();

    if (indicator != nullptr)
      indicator->setEventPattern(StatusIndicator::EventPattern::Reset);
  }

  void BluepadHub::taskStatusIndicator(void *param){
    while (1) {
      vTaskDelay(1);

      if (::BluepadHub.indicator != nullptr)
        ::BluepadHub.indicator->showStatusPattern();
      else
        vTaskDelay(100);
    }
  }

  void BluepadHub::setControlProfile(ControlProfile* _profile) {
    profile = _profile;
    if (profile != nullptr)
      profile->begin();
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

}

      