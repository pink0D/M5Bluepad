//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MKBLEAdvertiser.h"
#include "MKCryptoHelper.h"

#include <Arduino.h>
#include "BluepadHub.h"

uint8_t MKSeedArray[] = 
    {
        0xC1, 
        0xC2, 
        0xC3, 
        0xC4, 
        0xC5, 
    };

 uint8_t MKHeaderArray[] =
    {
        0x71,   // 0x71 (113)
        0x0f,   // 0x0f (15)
        0x55,   // 0x55 (85)
    };

  
int MKBLEAdvertiser::advertisingCount = 0;

static btstack_context_callback_registration_t update_callback_registration;

MKBLEAdvertiser::MKBLEAdvertiser() {
    if (advertisingCount > 0) {
        advertisementDisabled = true;
    }

    advertisingCount++;
}

void MKBLEAdvertiser::begin(uint16_t _manufacturer_id , 
            const uint8_t* _seedArray, int _seedArraySize, 
            const uint8_t* _headerArray , int _headerArraySize,
            uint8_t _CTXValue1, uint8_t _CTXValue2, 
            int _encryptedHeaderOffset, int _encryptedPacketLength) {

    if (advertisementDisabled) {
        Serial.println("WARNING: current platform does not allow multiple BLE Advertisements");
    }

    this->manufacturer_id = _manufacturer_id;
    this->encryptedHeaderOffset = _encryptedHeaderOffset;
    this->encryptedPacketLength = _encryptedPacketLength;
    this->CTXValue1 = _CTXValue1;
    this->CTXValue2 = _CTXValue2;

    if ( (_seedArray == nullptr) || (_seedArraySize == 0) ) {
        this->seedArray = MKSeedArray;
        this->seedArraySize = sizeof(MKSeedArray);
    } else {
        this->seedArray = _seedArray;
        this->seedArraySize = _seedArraySize;
    }

    if ( (_headerArray == nullptr) || (_headerArraySize == 0) ) {
        this->headerArray = MKHeaderArray;
        this->headerArraySize = sizeof(MKHeaderArray);
    } else {
        this->headerArray = _headerArray;
        this->headerArraySize = _headerArraySize;
    }
}

void MKBLEAdvertiser::connect(int _connect_duration) {
    this->connect_duration = _connect_duration;
    xTaskCreate(task, "connect_task", TASK_STACK_SIZE, this, 0, NULL);
}

void MKBLEAdvertiser::task(void *param){
    static_cast<MKBLEAdvertiser*>(param)->connect_task();
}

void MKBLEAdvertiser::connect_task(){
    
    isConnected = false;

    uint8_t payload[32];
    int payload_len = getConnectPayload(payload, sizeof(payload));

    if (payload_len > 0) {
        updateAdvertisement(payload, payload_len);
    }

    vTaskDelayMillis(connect_duration);
    isConnected = true;

    vTaskDelete(nullptr); // end task
}

void MKBLEAdvertiser::disconnect() {
    
    enable_adv = false;
    isConnected = false;

    run_btstack_callback();
}

void MKBLEAdvertiser::update() {
    if (!isConnected)
        return;

    uint8_t payload[32];
    int payload_len = getUpdatePayload(payload, sizeof(payload));
    
    if (payload_len > 0) {
        updateAdvertisement(payload, payload_len);
    }
}

void MKBLEAdvertiser::btstack_callback(void *context) {
    static_cast<MKBLEAdvertiser*>(context)->btstack_update_advertisement();
}

void MKBLEAdvertiser::btstack_update_advertisement() {

    if (enable_adv) {
        uint8_t adv_type = 0;
        bd_addr_t null_addr;
        memset(null_addr, 0, 6);

        gap_advertisements_set_params(32, 32, adv_type, 0, null_addr, 0x07, 0x00);
        gap_advertisements_set_data(adv_data_len, adv_data);
        gap_advertisements_enable(1);

    } else {
        gap_advertisements_enable(0);
    }
}


// encrypts payload and updates BLE advertisement    
void MKBLEAdvertiser::updateAdvertisement(uint8_t *payload, int payload_len) {
    if ( (advertisementDisabled) || (payload_len<=0) ) {
        return;
    }

    uint8_t encrypted_payload[32];
    int encrypted_payload_len = encryptPayload(payload, payload_len, encrypted_payload, sizeof(encrypted_payload));

    adv_data_len = encrypted_payload_len + 4 + 3;

    // check that encrypted payload together with headers does not exceed adv data buffer
    if (adv_data_len > sizeof(adv_data)) {
        Serial.println("WARNING: MKBLEAdvertiser::updateAdvertisement adv_data buffer too short");
        return;
    }

    // Flags
    adv_data[0] = 0x02;
    adv_data[1] = 0x01;
    adv_data[2] = 0x06;

    // Manufacturer Data
    adv_data[3] = (uint8_t) encrypted_payload_len + 3;                  // MF Data length 
    adv_data[4] = 0xFF;                                                 // Type = Manufacturer Data
    memcpy(&adv_data[5], &manufacturer_id, sizeof(manufacturer_id));    // company id
    memcpy(&adv_data[7], encrypted_payload, encrypted_payload_len);     // payload

    enable_adv = true;

    
    // all calls to btstack api need to be made from main thread
    // this can be done by registering a callback
    run_btstack_callback();
}

void MKBLEAdvertiser::run_btstack_callback() {
    update_callback_registration.callback = &btstack_callback;
    update_callback_registration.context = this;
    btstack_run_loop_execute_on_main_thread(&update_callback_registration);
}

int MKBLEAdvertiser::encryptPayload(uint8_t *payload, int payload_len, uint8_t *destination, int maxlen) {

    if (maxlen < encryptedPacketLength) {
        Serial.println("WARNING: MKBLEAdvertiser::encryptPayload destination buffer too short");
        return 0;
    }

    int encrypted_payload_len = MKCryptoHelper::encryptPayload(seedArray, seedArraySize, headerArray, headerArraySize, 
            payload, payload_len, encryptedHeaderOffset, CTXValue1, CTXValue2, destination, encryptedPacketLength);

    for (int index = encrypted_payload_len; index < encryptedPacketLength; index++)
    {
        destination[index] = (uint8_t)(index + 1);
    }

    return encryptedPacketLength;
}
