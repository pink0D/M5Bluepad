//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericChannel.h"

namespace bluepadhub {

  // global instances
  ChannelTypeMapperImpl ChannelTypeMapper;
  ServoUnitsMapperImpl ServoUnitsMapper;
  BluepadControllerInputMapperImpl BluepadControllerInputMapper;
  BluepadControllerButtonMapperImpl BluepadControllerButtonMapper;

  void GenericChannel::loadDefaultSettings() {

  }

  void GenericChannel::applySettings() {
    // reset channel value when settings are changed
  }

  bool GenericChannel::loadSettingsFromJson(JsonObject json) {

    return true;
  }

  JsonDocument GenericChannel::saveSettingsToJson() {
    
    JsonDocument json;

    return json;
  }

  void GenericChannel::getOutput(BluepadController *ctl, float *value, bool* brake) {

  }
  
}