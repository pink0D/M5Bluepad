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

          void setAntiJitter(double _antiJitter);

          static void resetIdleState() { idleState = true; };
          static bool isIdleState() { return idleState; };

      protected:

          void setLimits(double _limitMin, double _limitMax);
          double updateValue(double newValue);
          double getValue() { return value; };
          bool isZeroValue(double value);

      private:
      
          double antiJitter = 0.025;
          double limitMin = 0.0;
          double limitMax = 1.0;
          double value = 0.0;

          static inline bool idleState = false;
  };  

}

#endif
