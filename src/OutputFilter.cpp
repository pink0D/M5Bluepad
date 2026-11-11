//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "OutputFilter.h"

#include <Arduino.h>
#include <esp_timer.h>


namespace bluepadhub {

  void OutputFilter::setLimits(float _limitMin, float _limitMax) {
    if (_limitMin < 0)
      _limitMin = 0;

    if (_limitMax > 1.0)
      _limitMax = 1.0;

    limitMin = _limitMin;
    limitMax = _limitMax;
  }

  void OutputFilter::setAntiJitter(float _antiJitter) { 
    antiJitter = _antiJitter;
  }

  bool OutputFilter::isZeroValue(float value) {
    return abs(value) < antiJitter;
  }

  void OutputFilter::setAccelerationTime(int millis) {
    this->accelerationTime = millis;
  }

  float OutputFilter::updateValue(float newValue) {

    if (newValue < -1.0)
      newValue = -1.0;

    if (newValue > 1.0)
      newValue = 1.0;

    float calc_value = 0.0; // calculate new value
    long time_now = 0;
    
    // calculate positive value
    if (newValue > antiJitter)
      calc_value = limitMin + (limitMax - limitMin) * newValue;

    // calculate negative value
    if (newValue < -antiJitter)
      calc_value = -limitMin + (limitMax - limitMin) * newValue;

    if (accelerationTime > 0) {
      // limit motor output change rate

      time_now = esp_timer_get_time();
      float max_change = ((time_now - updateTime) / 1000.0f) / ((float)accelerationTime);

      if ( (calc_value > 0) && (calc_value > value + max_change) )
         calc_value = value + max_change;

      if ( (calc_value < 0) && (calc_value < value - max_change) )
         calc_value = value - max_change;
    }

    // set to negative limit if the value is below negative threshold
    if (calc_value < -limitMax+antiJitter)
      calc_value = -limitMax;

    // set to positive limit if the value is above positive threshold
    if (calc_value > limitMax-antiJitter)
      calc_value = limitMax;

    // set to zero if the value is close to it
    if (abs(calc_value) < antiJitter)
      calc_value = 0;

    // update final value if the change is over antiJitter
    if (abs(value-calc_value) > antiJitter) {
      value = calc_value;
      idleState = false;
    }

    updateTime = time_now;
    return value;
  }

}