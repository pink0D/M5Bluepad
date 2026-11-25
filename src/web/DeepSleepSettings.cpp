//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "DeepSleepSettings.h"
#include "BluepadHub.h"

namespace bluepadhub::web {
  
  bool DeepSleepSettings::loadSettingsFromJson(JsonDocument &json) {

    enableDeepSleep = json["enableDeepSleep"];

    return true;
  }

  JsonDocument DeepSleepSettings::saveSettingsToJson() {

    JsonDocument json;

    json["enableDeepSleep"] = enableDeepSleep;

    return json;
  }


}