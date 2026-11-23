//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "BluetoothSettings.h"
#include "BluepadHub.h"

namespace bluepadhub {
  
  bool BluetoothSettings::loadSettingsFromJson(JsonDocument &json) {

    ::BluepadHub.getProfile()->enablePairingAfterStartup = json["enablePairingAfterStartup"];

    return true;
  }

  JsonDocument BluetoothSettings::saveSettingsToJson() {

    JsonDocument json;

    json["enablePairingAfterStartup"] = ::BluepadHub.getProfile()->enablePairingAfterStartup;

    return json;
  }


}