//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#pragma once

#include <Arduino.h>

namespace bluepadhub {

    class DeepSleep {

        public:
            DeepSleep() = default;    

            void begin(bool setBluepadHubDeepSleep = true);
            void startDeepSleep();

        protected:
            virtual void afterStartup() {};
            virtual void beforeSleep() {};

        private:
            void task();
    };

}
