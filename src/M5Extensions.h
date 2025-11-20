//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _M5_EXTENSIONS_H_
#define _M5_EXTENSIONS_H_

#define M5_ATOMIC_MOTION_SERVO_COUNT  4
#define M5_ATOMIC_MOTION_MOTOR_COUNT  2
#define M5_UNIT_8SERVOS_COUNT         8

#define CHANNEL_SERVO_1 0
#define CHANNEL_SERVO_2 1
#define CHANNEL_SERVO_3 2
#define CHANNEL_SERVO_4 3

#define CHANNEL_MOTOR_1 0
#define CHANNEL_MOTOR_2 1

#define SERVO_ATOM_LITE_PORT_A    26
#define SERVO_ATOMIC_BASE_PORT_B  33
#define SERVO_ATOMIC_BASE_PORT_C  19

#include "M5AtomicMotionExt.h"
#include "M5HDriverBaseExt.h"
#include "M5Unit8ServosExt.h"
#include "ServoPWM.h"
#include "NeoPixelStatusIndicator.h"

class BluetoothPairingButtonHelper {
    public:
        void begin(unsigned long _short_click = 2500, unsigned long _long_click = 5000, int _button_pin = 39);
        
        static void button_task(void *param);

    private:

        void task();
        unsigned long short_click;
        unsigned long long_click;
        int button_pin;
};

extern BluetoothPairingButtonHelper BluetoothPairingButton;

#endif