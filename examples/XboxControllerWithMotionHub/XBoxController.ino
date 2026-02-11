/*
Based on Example from BluePad32 docs

This sketch can be used to test gamepad connection to the ESP32 
and dump controller's data to Serial Monitor 
to understand button codes and stick/trigger deadzones

1. Reboot the board after uploading sketch
2. The board will go to pairing mode at startup
3. Enter pairing mode on your gamepad and wait until the connection is established
4. Gamepad data is printed in Serial Monitor

*/

#include <MouldKingExtensions.h>
#include <M5Extensions.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// MK 4.0 module
MouldKing40Ext MK;   //MK 6.0 is also supported (see MouldKing60Ext class)

int controllerStickDeadzoneLow = 50; 
int controllerStickDeadzoneHigh = 500;

int controllerTriggerDeadzoneLow = 5;
int controllerTriggerDeadzoneHigh = 1000;

M5AtomLiteIndicator   AtomLiteIndicator;  // RGB LED status indication
M5AtomicMotionExt     AtomicMotionExt;    // M5AtomicMotion extension

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, L1:%d, R1:%d, "
        "misc: 0x%02x, A:%d, B:%d, X:%d, Y:%d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->l1(),
        ctl->r1(),
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->a(),
        ctl->b(),
        ctl->x(),
        ctl->y()
    );
}

void updateServos(ControllerPtr ctl) {
    double steer = normalizeStickInput(ctl->axisX());       // L stick to steer

    // update servo position
    AtomicMotionExt.updateServo(SERVO_1, normalizeStickInput(ctl->axisX()));
    AtomicMotionExt.updateServo(SERVO_3, normalizeStickInput(ctl->axisY()));
    AtomicMotionExt.updateServo(SERVO_2, normalizeStickInput(ctl->axisRX()));
    AtomicMotionExt.updateServo(SERVO_4, normalizeStickInput(ctl->axisRY()));

}

void sendMKCommand(ControllerPtr ctl) {
    double speed = normalizeTriggerInput( (ctl->throttle() - ctl->brake()) );

    MK.setInstanceNumber(0);
    // update motor speed: value > 0 (forward), value < 0 (reverse), value = 0 (coast)
    //MK.updateMotorSpeed(MOTOR_A, (speed * -1.0));
    MK.updateMotorSpeed(MOTOR_A, -speed );
    //MK.updateMotorSpeed(MOTOR_D, ( normalizeStickInput(ctl->axisX()) * -1.0));

    MK.setInstanceNumber(1);
    MK.updateMotorSpeed(MOTOR_A, speed);

    // all channel outputs are combined into a single Bluetooth packet for MK module, 
    // so the the outgoing payload is refreshed once after all channels where updated
    MK.applyUpdates();
}

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            //Set LED indication
            AtomLiteIndicator.setStatusPattern(bluepadhub::StatusIndicator::StatusPattern::Connected);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            //Set LED indication
            AtomLiteIndicator.setStatusPattern(bluepadhub::StatusIndicator::StatusPattern::Warning);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                //dumpGamepad(myController);
                updateServos(myController);
                sendMKCommand(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

double normalizeTriggerInput(int input) {

    if (std::abs(input) < controllerTriggerDeadzoneLow)
        return 0;

    if (input > controllerTriggerDeadzoneHigh)
        return 1;
    if (input < -controllerTriggerDeadzoneHigh)
        return -1;

    return ((double)input) / 1024.0 ;
}


double normalizeStickInput(int input) {

    if (abs(input) < controllerStickDeadzoneLow)
        return 0;

    double sign = input > 0 ? 1.0 : -1.0;
    if (abs(input) > controllerStickDeadzoneHigh)
        return sign;

    return ((double)input) / 512.0;
}

void setup() {

    Serial.begin(115200);

    // use Atom Lite RGB LED for status indication
    AtomLiteIndicator.setBrightness(20);
    AtomLiteIndicator.begin();
    //Set LED indication
    AtomLiteIndicator.setStatusPattern(bluepadhub::StatusIndicator::StatusPattern::Pairing);

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    MK.connect();

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);

    while(!AtomicMotionExt.begin()) {
        Serial.println("Atomic Motion begin failed");
        //Set LED indication
        AtomLiteIndicator.setStatusPattern(bluepadhub::StatusIndicator::StatusPattern::Warning);
        delay(500);
    }

    // specify 500..2500us pulse for 360-servo
    // if no pulse range is specified, then default 1000..2000us will be used (180-servo)
    //AtomicMotionExt.setServoPulseRange(SERVO_1, 500, 2500);
    AtomicMotionExt.setServoPulseRange(SERVO_1, 1000, 2000);
    AtomicMotionExt.setServoPulseRange(SERVO_2, 500, 2500);
    AtomicMotionExt.setServoPulseRange(SERVO_3, 500, 2500);
    AtomicMotionExt.setServoPulseRange(SERVO_4, 500, 2500);

    // alternatevely, you can specify servo rotation angle in degrees
    // this will be recalculated to corresponding pulse range internally
    // AtomicMotionExt.setServoMaxAngle(SERVO_1, 45);
    // AtomicMotionExt.setServoMaxAngle(SERVO_2, 90);

    // PWM duty cycle for M1 & M2 can be limited from 0 to 100% (default is full range)
    // usually this is not needed, but some motors start only when PWM is more than 25-30%
    // AtomicMotionExt.setMotorLimits(MOTOR_1, 0.25, 1.0);

    // if necessary, the upper PWM limit can be also specified to limit maximum motor power 
    // AtomicMotionExt.setMotorLimits(MOTOR_2, 0.25, 0.75);

    // M5.dis.drawpix(0, 0x00ff00);
    Serial.println("Setup finished");
}

void loop() {
    // get updates from controller

    bool dataUpdated = BP32.update();
    if (dataUpdated)
    {
        processControllers();
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}