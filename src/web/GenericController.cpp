//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericController.h"

namespace bluepadhub {

  void GenericController::loadDefaultSettings() {

    if (channels != nullptr)
      delete [] channels;

    int count = getChannelCount();

    if (count > 0) {
      channels = new GenericChannel[count];
    }

    // ensures channels have diffrerent index across all controllers
    static int channel_index = 0;

    for (int i=0; i<getChannelCount(); i++) {
      channels[i].loadDefaultSettings(channel_index++);
      setChannelDefaults(i,&channels[i]);
    }
  }

  void GenericController::applySettings() {
    
    for (int i=0; i<getChannelCount(); i++) {
      channels[i].applySettings();
      applyOutputSettings(i, &channels[i]);
    }
  }

  bool GenericController::loadSettingsFromJson(JsonDocument &json) {

    for (int i=0; i<getChannelCount(); i++) {
      channels[i].loadSettingsFromJson(json["channels"][i]);
    }

    return true;
  }

  JsonDocument GenericController::saveSettingsToJson() {

    JsonDocument json, jsonChannels;

    for (int i=0; i<getChannelCount(); i++) {
      jsonChannels[i] = channels[i].saveSettingsToJson();
    }

    json["channelCount"] = getChannelCount();
    json["channels"] = jsonChannels;

    return json;
  }

  void GenericController::processBluepadController(Profile *profile, BluepadController *ctl) {

    for (int i=0; i<getChannelCount(); i++) {
      
      float value = 0;
      bool brake = false;

      channels[i].getOutput(profile, ctl, &value, &brake);
      updateOutput(i, value, brake);
    }
  }

}