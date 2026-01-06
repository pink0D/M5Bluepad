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

namespace bluepadhub {

  double VoltageMonitor::getAverageFromSamples(double *samples, int sampleCount) {

    int num_samples = 0;
    double sum = 0;

    for (int i=0; i<sampleCount; i++) {
      if (samples[i] > 0.5) {
        num_samples++;
        sum += samples[i];
      }
    }

    if (num_samples > 0) {
      return sum / ( (double)num_samples);
    } 

    return 0;
  }

  void VoltageMonitor::update() {
    long time_now = esp_timer_get_time();

    if (time_now < time_next_update) 
      return;

    time_next_update = time_now + update_interval; // update voltage every 500ms

    // get multiple ADC measurements
    for (int i=0; i<voltageReadCount; i++) {

      voltageSamples[voltageIndex] = readVoltage();
      voltageIndex = (voltageIndex + 1) % voltageSampleCount;
    }

    voltage = getAverageFromSamples(voltageSamples, voltageSampleCount);
    lowBattery = isLowVoltage(voltage);

    // derived classes can add some other status processing
    updateStatus();

    if (printVoltageEnabled) {
      Serial.print("Voltage: ");
      Serial.print(voltage);
      Serial.println("V");
    }
  }

  bool VoltageMonitor::isLowVoltage(double voltage) {

    if (voltage > 0) {

      if (voltage < 6) // never run motors powered from USB port
        return true; 
      
      if ( (voltage > 6) && (voltage < 9) ) // looks like a 2S battery
        return voltage < 2 * minCellVoltage;

      if ( (voltage > 9) && (voltage < 14) ) // looks like a 3S battery
        return voltage < 3 * minCellVoltage;
    
      return true; // unknown battery type, better don't try

    } else  
      return false; // return OK until voltage is read
  }

}