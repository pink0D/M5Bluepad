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

  int MouldKingController::getChannelCount() {

    if (moduleType == MouldKingModuleType::MK40)
      return 4;

    if (moduleType == MouldKingModuleType::MK60)
      return 6;

    if (moduleType == MouldKingModuleType::MK40x3)
      return 12;

    return 0;
  }


  void MouldKingController::loadDefaultSettings() {

    moduleType = MouldKingModuleType::None;

    MultiMotorController::loadDefaultSettings();
  }

  void MouldKingController::applySettings() {

    if (mk != nullptr) {

        // stop BLE advertising if no new MK module is going to be set up. 
        // otherwise, BLE advertising will be continued by new MK module
        if (moduleType == MouldKingModuleType::None) {
          mk->disconnect();
        }

        delete mk;
        mk = nullptr;
    }

    MouldKingMulti40Ext *mk40multi;
    MouldKing40Ext *mk40;
    MouldKing60Ext *mk60;

    switch (moduleType) {

        case MouldKingModuleType::None:
            return;

        case MouldKingModuleType::MK40:
            mk40 = new MouldKing40Ext();
            mk = mk40;
            setUnit(mk40->getMultiMotorUnit());
            break;

        case MouldKingModuleType::MK40x3:
            mk40multi = new MouldKingMulti40Ext();
            mk = mk40multi;
            setUnit(mk40multi->getMultiMotorUnit());
            break;

        case MouldKingModuleType::MK60:
            mk60 = new MouldKing60Ext();
            mk = mk60;
            setUnit(mk60->getMultiMotorUnit());
            break;
    }

    mk->connectAsync();  

    MultiMotorController::applySettings();

  }

  void MouldKingController::stop() {

      MultiMotorController::stop();

      if (mk != nullptr) {
        mk->applyUpdates();
      }
  }

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