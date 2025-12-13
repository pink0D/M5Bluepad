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

#include <MouldKingino.h>

#include "MultiMotorUnit.h"

template<class BaseClass, int numChannels>
class MKModuleExt : public BaseClass, public bluepadhub::MultiMotorUnit<numChannels> {

    public:
        // override updateMotorOutput from MouldKingino so the value goes through OutputFilter in MultiMotorUnit
        virtual void updateMotorOutput(int channel, double normalized_value) {
            bluepadhub::MultiMotorUnit<numChannels>::updateMotorSpeed(channel, normalized_value);
        };

        virtual void stopMotors() {
            bluepadhub::MultiMotorUnit<numChannels>::stopMotors();
            BaseClass::applyUpdates();
        };

        virtual void stop() {
            bluepadhub::MultiMotorUnit<numChannels>::stop();
            BaseClass::applyUpdates();
        };

    private:
        virtual void outputMotorSpeed(int channel, double normalized_speed) {            
            BaseClass::updateMotorOutput(channel, normalized_speed);
        };

};

typedef MKModuleExt<MouldKing40, 4> MouldKing40Ext;

typedef MKModuleExt<MouldKing60, 6> MouldKing60Ext;

#endif
