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

#define SERVO_1 0
#define SERVO_2 1
#define SERVO_3 2
#define SERVO_4 3

#define MOTOR_1 0
#define MOTOR_2 1

#define SERVO_ATOM_LITE_PORT_A     26
#define SERVO_ATOM_LITE_PORT_A1    26
#define SERVO_ATOM_LITE_PORT_A2    32

#define SERVO_ATOMIC_BASE_PORT_B   33
#define SERVO_ATOMIC_BASE_PORT_B1  33
#define SERVO_ATOMIC_BASE_PORT_B2  23

#define SERVO_ATOMIC_BASE_PORT_C   19
#define SERVO_ATOMIC_BASE_PORT_C1  19
#define SERVO_ATOMIC_BASE_PORT_C2  22

#include "M5AtomicMotionExt.h"
#include "M5HDriverBaseExt.h"
#include "M5Unit8ServosExt.h"
#include "M5Atom.h"
#include "ServoPWM.h"

typedef bluepadhub::ServoPWM M5GroveServo;


#endif