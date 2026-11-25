//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MouldKingController.h"

namespace bluepadhub::web {
  
  MouldKingModuleTypeMapperImpl MouldKingModuleTypeMapper;


  void MouldKingController::loadDefaultSettings() {

    moduleType = MouldKingModuleType::None;

    MultiMotorController::loadDefaultSettings();
  }

  void MouldKingController::applySettings() {

    if (mk != nullptr) {

        // stop BLE advertising if no new MK module is going to be set up. 
        // otherwise, BLE advertising will be updated with new data by new MK module
        if (moduleType == MouldKingModuleType::None) {
          mk->disconnect();
        }

        delete mk;
        mk = nullptr;
    }

    switch (moduleType) {

        case MouldKingModuleType::None:
            return;

        case MouldKingModuleType::MK40:
            mk = new MouldKing40();
            break;

        case MouldKingModuleType::MK40x3:
            mk = new MouldKingMulti40();
            break;

        case MouldKingModuleType::MK60:
            mk = new MouldKing60();
            break;
    }

    mk->connectAsync();   

    MultiMotorController::applySettings();

  }

  void MouldKingController::stopMotors() {

      MKMaxMultiMotorUnit::stopMotors();
      if (mk != nullptr) {
        mk->applyUpdates();
      }
  };

  void MouldKingController::outputMotorSpeed(int channel, float normalized_speed) { 
    if (mk != nullptr) {
        mk->updateMotorOutput(channel, normalized_speed);
    }           
  };


  void MouldKingController::processBluepadController(Profile *profile, BluepadController *ctl) {

    if (mk != nullptr) {
      MultiMotorController::processBluepadController(profile, ctl);
      mk->applyUpdates();
    }

  }  

  bool MouldKingController::loadSettingsFromJson(JsonDocument &json) {

    moduleType = MouldKingModuleTypeMapper.mapStringToEnum(json["moduleType"]);

    return MultiMotorController::loadSettingsFromJson(json);
  }

  JsonDocument MouldKingController::saveSettingsToJson() {

    JsonDocument json = MultiMotorController::saveSettingsToJson();

    json["moduleType"] = MouldKingModuleTypeMapper.mapEnumToString(moduleType);
    json["advancedChannels"] = advancedChannels;
    json["enableMultiModule"] = multiModuleEnabled;

    return json;
  }


}