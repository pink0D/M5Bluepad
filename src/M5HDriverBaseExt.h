//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _M5HDRIVER_BASE_EXT_H_
#define _M5HDRIVER_BASE_EXT_H_

#include "MotorDriverHBridge.h"
#include "VoltageMonitor.h"

class M5HDriverBaseExt : public MotorDriverHBridge, public VoltageMonitor {

    public:
        void begin(int pwm_frequency = 1000);

        bool isFault() {
            return fault;
        };

    private:
        bool fault = false;

        virtual double readVoltage();
};

#endif