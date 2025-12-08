//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MOULD_KING_H
#define _MOULD_KING_H

#include "MK40Advertiser.h"
#include "MK60Advertiser.h"
#include "MultiMotorUnit.h"

#define MODULE_1 0
#define MODULE_2 1
#define MODULE_3 2

#define CHANNEL_A 0
#define CHANNEL_B 1
#define CHANNEL_C 2
#define CHANNEL_D 3
#define CHANNEL_E 4
#define CHANNEL_F 5

template<class AdvertiserClass, int numChannels>
class MKModule : public bluepadhub::MultiMotorUnit<numChannels> {
    public:
        MKModule(int instanceNum = 0, bool immediateUpdate = false) {
            this->advertiser = AdvertiserClass::getAdvertiser(instanceNum);
            this->instanceNum = instanceNum;
            this->immediateUpdate = immediateUpdate;
        };

        void setInstanceNumber(int instanceNum) {
            this->instanceNum = instanceNum;
            this->advertiser->setInstanceNumber(instanceNum);
        }

        void setImmediateUpdates(bool immediateUpdate) {
            this->immediateUpdate = immediateUpdate;
        }

        virtual void begin(uint16_t manufacturer_id = 0xFFF0, 
            const uint8_t* seedArray = nullptr, int seedArraySize = 0, 
            const uint8_t* headerArray = nullptr, int headerArraySize = 0,
            uint8_t CTXValue1 = 0x3f, uint8_t CTXValue2 = 0x25, 
            int encryptedHeaderOffset = 15, int encryptedPacketLength = 24) {

            advertiser->begin(manufacturer_id, 
                seedArray, seedArraySize, headerArray, headerArraySize,
                CTXValue1, CTXValue2, encryptedHeaderOffset, encryptedPacketLength);
        }

        void connect(int connect_duration = 1000) {
            advertiser->connect(connect_duration);
        };

        void disconnect() {
            advertiser->disconnect();
        };

        void applyUpdates() {
            advertiser->update();
        };


    private:
        virtual void outputMotorSpeed(int channel, double normalized_speed) {
            
            advertiser->setChannelValue(instanceNum, channel, normalized_speed);

            if (immediateUpdate)
                applyUpdates();
        };

        int instanceNum = 0;
        bool immediateUpdate = false;
        AdvertiserClass *advertiser;

};

typedef MKModule<MK40Advertiser, 4> MouldKing40;

typedef MKModule<MK60Advertiser, 6> MouldKing60;

#endif