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

  float OutputFilter::updateValue(float newValue) {

    if (newValue < -1.0)
      newValue = -1.0;

    if (newValue > 1.0)
      newValue = 1.0;

    float calc_value = 0.0; // calculate new value
    
    if (newValue > antiJitter)
      calc_value = limitMin + (limitMax - limitMin) * newValue;

    if (newValue < -antiJitter)
      calc_value = -limitMin + (limitMax - limitMin) * newValue;

    if (calc_value < -limitMax+antiJitter)
      calc_value = -limitMax;

    if (calc_value > limitMax-antiJitter)
      calc_value = limitMax;

    if (abs(calc_value) < antiJitter)
      calc_value = 0;

    if (abs(value-calc_value) > antiJitter) {
      value = calc_value;
      idleState = false;
    }

    return value;
  }

}