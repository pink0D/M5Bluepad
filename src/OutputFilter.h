//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _OUTPUT_FILTER_H_
#define _OUTPUT_FILTER_H_

namespace bluepadhub {

  class OutputFilter {

      public:
          OutputFilter() {};    

          void setAntiJitter(float _antiJitter);

          static void resetIdleState() { idleState = true; };
          static bool isIdleState() { return idleState; };

      protected:

          void setLimits(float _limitMin, float _limitMax);
          float updateValue(float newValue);
          float getValue() { return value; };
          bool isZeroValue(float value);

      private:
      
          float antiJitter = 0.025;
          float limitMin = 0.0;
          float limitMax = 1.0;
          float value = 0.0;

          static inline bool idleState = false;
  };  

}

#endif
