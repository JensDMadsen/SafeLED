#ifndef ARDUINOSAFELEDSTATS_H
#define ARDUINOSAFELEDSTATS_H

#include "ArduinoSafeLED.h"

class ArduinoSafeLEDstats : public ArduinoSafeLED {
protected:
  uint32_t _errCount = 0;                           // LED verification error counter.

public:
  using ArduinoSafeLED::ArduinoSafeLED;             // Inherit constructor(s) from ArduinoSafeLED.

  uint32_t errCount() const { return _errCount;   } // Returns LED verification error counter.
  void clearErrCount()      { _errCount = 0;      } // Clears LED verification error counter.

protected:
  void ledError(uint8_t state,uint16_t adc) override { // Called when LED verification fails.
    _errCount++;                                    // Count LED verification failure.
  }
};

#endif