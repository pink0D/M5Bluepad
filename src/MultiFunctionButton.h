//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MULTI_FUNCTION_BUTTON_H_
#define _MULTI_FUNCTION_BUTTON_H_

#include <Arduino.h>

namespace bluepadhub {

    class MultiFunctionButton {
        public:
            MultiFunctionButton() {};

            enum class EventType {Click, Hold1, Hold2};
            enum class ButtonState {Pressed, Released};

            void setTimings(unsigned long _click_time, unsigned long _hold1_time, unsigned long _hold2_time);
            void begin(int _button_pin, int _pin_mode = INPUT_PULLUP);

        protected:
            virtual int readButtonState(int pin);

        private:

            void task();
            unsigned long click_time = 500;
            unsigned long hold2_time = 5000;
            unsigned long hold1_time = 10000;
            int button_pin;
    };

}

#endif