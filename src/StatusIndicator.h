//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _STATUS_INDICATOR_H_
#define _STATUS_INDICATOR_H_

namespace bluepadhub {

  class StatusIndicator {
    public:
      StatusIndicator() {};

      enum class StatusPattern {None, Idle, Pairing, Connected, Error, Warning, PowerOff};
      enum class EventPattern {None, Reset, Success, Error, Warning, Info, Select};

      virtual void begin(bool setBluepadHubStatusIndicator = true);

      virtual void stop() {
        isStopped = true;
        clear();
      };

      virtual void clear() {};

      void setStatusPattern(StatusPattern statusPattern) {
        
        if (isStopped)
          return;

        if (this->statusPattern != statusPattern) {
          statusPatternPrevious = this->statusPattern;
        }

        this->statusPattern = statusPattern;
      };

      void restorePreviousStatusPattern() {
        if (statusPatternPrevious != StatusPattern::None) {
          this->statusPattern = statusPatternPrevious;
        }
      }

      void setPowerOffStatus() {
        setStatusPattern(StatusPattern::PowerOff);
      };

      void setErrorStatus() {
        if (statusPattern != StatusPattern::PowerOff) {
          setStatusPattern(StatusPattern::Error);
        }
      };

      void clearErrorStatus() {
        if (statusPattern == StatusPattern::Error) {
          restorePreviousStatusPattern();
        }
      };

      void setWarningStatus() {
        if (statusPattern != StatusPattern::PowerOff) {
          setStatusPattern(StatusPattern::Warning);
        }
      };

      void clearWarningStatus() {
        if (statusPattern == StatusPattern::Warning) {
          restorePreviousStatusPattern();
        }
      };

      void setEventPattern(EventPattern eventPattern) {
        if (isStopped)
          return;

        this->eventPattern = eventPattern;
      };  

      virtual void showStatusPattern();
      virtual void showEventPattern();

    protected:
      StatusPattern statusPattern = StatusPattern::None;
      StatusPattern statusPatternPrevious = StatusPattern::None;
      EventPattern eventPattern = EventPattern::None;

      void patternDelayMillis(int timeout);

    private:
      bool isStopped = false;

      void task();
  };  

}

#endif
