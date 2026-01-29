//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//


#include <BluepadHub.h>
#include <UniProfile.h>
#include <M5Extensions.h>
#include <MouldKingExtensions.h>

template <class MKBase>
class MouldKingUniBase: 
  public MKBase, 
  public bluepadhub::MultiServoUnit<1, M5GroveServo>, 
  public bluepadhub::Profile 
{

  private:

    M5AtomLiteIndicator   AtomLiteIndicator;
    M5AtomLiteButton      AtomLiteButton;
    M5HDriverBaseExt      HDriverBaseExt;              

  public:  

    int getNumMotors() { 
      return MKBase::getChannelCount(); 
    };

    int getNumServos() { 
      return 1; 
    };

    virtual void setup() {
    
      AtomLiteIndicator.setBrightness(20);
      AtomLiteIndicator.begin();
          
      AtomLiteButton.begin();

      // enablePairingAfterStartup = true;
      
      // controllerStickDeadzoneLow = 50;        // 0 = lowest value for stick input
      // controllerStickDeadzoneHigh = 500;      // 512 = highest value for stick input
      // controllerTriggerDeadzoneLow = 5;       // 0 = lowest value for trigger input
      // controllerTriggerDeadzoneHigh = 1000;   // 1024 = highest value for trigger input 

      this->servo(0)->begin(SERVO_ATOM_LITE_PORT_A);

      HDriverBaseExt.begin(); 
    };

    virtual void afterSetup() {
      MKBase::connectAsync();
    };  

    virtual void processBluepadController(BluepadController* ctl) {
      MKBase::applyUpdates();
    };

    void failsafe() {
      this->stopMotors();
      this->stopServos();
    };

    double getVoltage() {
      return HDriverBaseExt.getVoltage();
    };

    bool isLowBatteryState() {
      return HDriverBaseExt.isLowBattery();
    };

    void idleTimer() {
      //MKBase::disconnect();
    };

    // Atom Lite button click
    void processButtonClick() {
      
      static int instanceNum = 0;

      // set which MK module is controlled - up to three modules can be selected
      MKBase::setInstanceNumber( (++instanceNum) % 3 ); 
      MKBase::connectAsync(); // broadcast connect message to module

      // indicate event
      AtomLiteIndicator.setEventPattern(bluepadhub::StatusIndicator::EventPattern::Select);
    }

};

template <class MKBase>
using MouldKingUniProfile = bluepadhub::UniProfile<MouldKingUniBase<MKBase>>;

MouldKingUniProfile<MouldKing40Ext> uni;

//  Arduino setup function
void setup() {
  uni.setDefaultDeviceName("MouldKingAdapter");
  uni.begin();
}

// Arduino loop function
void loop() {
  uni.update();
}
