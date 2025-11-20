//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "VoltageMonitor.h"
#include <esp_timer.h>
#include <Arduino.h>

void VoltageMonitor::update() {
  long time_now = esp_timer_get_time();

  if (time_now < time_next_update) 
    return;

  time_next_update = time_now + update_interval; // update voltage every 500ms

  voltage = readVoltage();
  lowBattery = isLowVoltage(voltage);

  if (printVoltageEnabled) {
    Serial.println(voltage);
  }
}

bool VoltageMonitor::isLowVoltage(double voltage) {

  if (voltage > 0) {

    if (voltage < 6.4) // never run motors powered from USB port
      return true; 
    
    if ( (voltage > 6.4) && (voltage < 8.9) ) // looks like a 2S battery
      return voltage < 2 * minCellVoltage;

    if ( (voltage > 9.6) && (voltage < 12.9) ) // looks like a 3S battery
      return voltage < 3 * minCellVoltage;
  
    return true; // unknown battery type, better don't try

  } else  
    return false; // return OK until voltage is read
}