//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "M5Atom.h"

#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>

void M5AtomLiteIndicator::begin(int16_t pin_rgb, int16_t pin_ir, bool setBluepadHubStatusIndicator){
    
    bluepadhub::NeoPixelStatusIndicator::begin(pin_rgb, setBluepadHubStatusIndicator);

    pinMode(pin_ir, OUTPUT);
    digitalWrite(pin_ir, LOW);
};    

void M5AtomDeepSleep::afterStartup() {
    
    // go to deep sleep if startup was caused by connecting battery
    if ( (getCheckWakeUpReason()) && (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1) ) {
        Serial.println("Startup not by ext1, going to deep sleep");
        startDeepSleep();
    }
    
    // proceed only if startup was caused by ext1 (button click)

    rtc_gpio_hold_dis(GPIO_NUM_12);
    //rtc_gpio_hold_dis(GPIO_NUM_19);
    //rtc_gpio_hold_dis(GPIO_NUM_21);
    //rtc_gpio_hold_dis(GPIO_NUM_22);
    //rtc_gpio_hold_dis(GPIO_NUM_23);
    rtc_gpio_hold_dis(GPIO_NUM_25);
    rtc_gpio_hold_dis(GPIO_NUM_26);
    rtc_gpio_hold_dis(GPIO_NUM_27);
    rtc_gpio_hold_dis(GPIO_NUM_32);
    rtc_gpio_hold_dis(GPIO_NUM_33);
    rtc_gpio_hold_dis(GPIO_NUM_34);
    rtc_gpio_hold_dis(GPIO_NUM_39);
}

void M5AtomDeepSleep::beforeSleep() {

    rtc_gpio_isolate(GPIO_NUM_12);
    //rtc_gpio_isolate(GPIO_NUM_19);
    //rtc_gpio_isolate(GPIO_NUM_21);
    //rtc_gpio_isolate(GPIO_NUM_22);
    //rtc_gpio_isolate(GPIO_NUM_23);
    rtc_gpio_isolate(GPIO_NUM_25);
    rtc_gpio_isolate(GPIO_NUM_26);
    rtc_gpio_isolate(GPIO_NUM_27);
    rtc_gpio_isolate(GPIO_NUM_32);
    rtc_gpio_isolate(GPIO_NUM_33);
    rtc_gpio_isolate(GPIO_NUM_34);

    // button wakeup
    esp_sleep_enable_ext1_wakeup(1ULL << 39, ESP_EXT1_WAKEUP_ALL_LOW);  

    // timer wakeup
    esp_sleep_enable_timer_wakeup(::BluepadHub.getProfile()->deepSleepTimeout*1000);

    // start deep sleep
    esp_deep_sleep_start();
}

