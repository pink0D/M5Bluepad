//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "GenericChannel.h"

namespace bluepadhub::web {

  // global instances
  ChannelTypeMapperImpl ChannelTypeMapper;
  ServoUnitsMapperImpl ServoUnitsMapper;
  BluepadControllerInputMapperImpl BluepadControllerInputMapper;
  BluepadControllerButtonMapperImpl BluepadControllerButtonMapper;

  void GenericChannel::loadDefaultSettings(int channel) {

    channel_index = channel;
    label = "";
    type = ChannelType::Disabled;
    input = BluepadControllerInput::NullInput;
    invertInput = false;
    reverseButton = BluepadControllerButton::NullButton;
    brake = BluepadControllerInput::NullInput;
    brakeTimeout = 0;
    button1 = BluepadControllerButton::NullButton;
    button2 = BluepadControllerButton::NullButton;
    buttonStop = BluepadControllerButton::NullButton;
    sticky = false;
    stepCount = 0;
    minPower = 0;
    maxPower = 100;
    softStartTime = 0;
    servoUnits = ServoUnits::Angle;
    servoMin = 1000;
    servoMax = 2000;
    servoMaxAngle = 90;
    servoCenterPos = 0;
  }

  bool GenericChannel::loadSettingsFromJson(JsonObject json) {

    type = ChannelTypeMapper.mapStringToEnum(json["type"]);
    input = BluepadControllerInputMapper.mapStringToEnum(json["input"]);
    invertInput = json["invertInput"];
    reverseButton = BluepadControllerButtonMapper.mapStringToEnum(json["reverseButton"]);
    brake = BluepadControllerInputMapper.mapStringToEnum(json["brake"]);
    brakeTimeout = json["brakeTimeout"];
    button1 = BluepadControllerButtonMapper.mapStringToEnum(json["button1"]);
    button2 = BluepadControllerButtonMapper.mapStringToEnum(json["button2"]);
    buttonStop = BluepadControllerButtonMapper.mapStringToEnum(json["buttonStop"]);
    sticky = json["sticky"];
    stepCount = json["stepCount"];
    minPower = json["minPower"];
    maxPower = json["maxPower"];
    softStartTime = json["softStartTime"];
    servoUnits = ServoUnitsMapper.mapStringToEnum(json["servoUnits"]);
    servoMin = json["servoMin"];
    servoMax = json["servoMax"];
    servoMaxAngle = json["servoMaxAngle"];
    servoCenterPos = json["servoCenterPos"];

    return true;
  }

  JsonDocument GenericChannel::saveSettingsToJson() {
    
    JsonDocument json;

    json["label"] = label;
    json["type"] = ChannelTypeMapper.mapEnumToString(type);
    json["input"] = BluepadControllerInputMapper.mapEnumToString(input);
    json["invertInput"] = invertInput;
    json["reverseButton"] = BluepadControllerButtonMapper.mapEnumToString(reverseButton);
    json["brake"] = BluepadControllerInputMapper.mapEnumToString(brake);
    json["brakeTimeout"] = brakeTimeout; 
    json["button1"] = BluepadControllerButtonMapper.mapEnumToString(button1);
    json["button2"] = BluepadControllerButtonMapper.mapEnumToString(button2);
    json["buttonStop"] = BluepadControllerButtonMapper.mapEnumToString(buttonStop);
    json["sticky"] = sticky;
    json["stepCount"] = stepCount;
    json["minPower"] = minPower;
    json["maxPower"] = maxPower;
    json["softStartTime"] = softStartTime;
    json["servoUnits"] = ServoUnitsMapper.mapEnumToString(servoUnits);
    json["servoMin"] = servoMin; 
    json["servoMax"] = servoMax;
    json["servoMaxAngle"] = servoMaxAngle; 
    json["servoCenterPos"] = servoCenterPos;

    return json;
  }

  void GenericChannel::applySettings() {
    // reset channel value when settings are changed

    direction = 0;
    timer_brake = 0;
    step = 0;

    if (stepCount < 1)
      stepCount = 1;
  }

  void GenericChannel::getOutput(Profile *profile, BluepadController *ctl, float *pOutValue, bool* pOutBrake) {

    float out_value = 0;
    bool out_brake = 0;

    if (type == ChannelType::Analog) {

      // set input value
      out_value = profile->getInputValue(ctl, input);
      
      // invert value if this option is set
      if (invertInput) {
        out_value = -out_value;
      }

      // invert value when reverse button is pressed
      if (profile->getButtonValue(ctl, reverseButton)) {
        out_value = -out_value;
      }

      // set brake flag
      out_brake = profile->getInputValue(ctl, brake) > 0;  

      // braking with reverse enabled
      if (brakeTimeout > 0) {

        // remember current direction (state variable)
        if (out_value > 0)
          direction = 1.0;
        else if (out_value < 0)
          direction = -1.0;

        unsigned long currentTime = esp_timer_get_time();

        // set brake timer when button is pressed (1st time) 
        if (profile->wasClickedImpl(__LINE__ + channel_index*1000, out_brake)) {
          timer_brake = currentTime + brakeTimeout*1000;
        }

        // reverse motor speed during brake timeout
        if (currentTime < timer_brake) {
          out_value = -0.15 * direction;
        } 
        else if (out_brake) {
          out_value = 0;
        }

        // brake flag shouldn't be used for output
        out_brake = false;
      }

    }

    if (type == ChannelType::Buttons) {
      
      bool up = profile->getButtonValue(ctl, button1);
      bool down = profile->getButtonValue(ctl, button2);

      if (sticky) {

        if (profile->wasClickedImpl(__LINE__ + channel_index*1000, up)) {
          direction = direction > 0 ? 0.0 : 1.0;          
        }

        if (profile->wasClickedImpl(__LINE__ + channel_index*1000, down)) {
          direction = direction < 0 ? 0.0 : -1.0;          
        }

        out_value = direction;

      } else {
        out_value = up ? 1.0 : down ? -1.0 : 0;
      }
    }

    if (type == ChannelType::Stepper) {

      bool up = profile->getButtonValue(ctl, button1);
      bool down = profile->getButtonValue(ctl, button2);
      bool stop = profile->getButtonValue(ctl, buttonStop);

      if (profile->wasClickedImpl(__LINE__ + channel_index*1000, up)) {
        step += 1;      
        if (step > stepCount)
          step = stepCount;  
      }

      if (profile->wasClickedImpl(__LINE__ + channel_index*1000, down)) {
        step -= 1;     
        if (step < -stepCount)
          step = -stepCount;    
      }

      if (profile->wasClickedImpl(__LINE__ + channel_index*1000, stop)) {
        step = 0;
      }

      out_value = ((float) step) / ((float) stepCount);
    }

    *pOutValue = out_value;
    *pOutBrake = out_brake;
  }
  
}