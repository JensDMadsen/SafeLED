//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  SafeLED - Arduino LED library with polarity detection and runtime verification.                                                     //
//  Copyright (C) 2026 Jens Dyekjær Madsen                                                                                              //
//                                                                                                                                      //
//  Filename: ArduinoSafeLEDstats.h   - Optional statistics and error counter layer.                                                    //
//                                                                                                                                      //
//  Description:                                                                                                                        //
//    Library for mono and bi-color status LEDs with automatic polarity detection, voltage verification, runtime diagnostics,           //
//    fault detection, and watchdog compatible validation. Suitable for alarm indicators, warning lamps, brake lights,                  //
//    industrial status indicators, and other applications where LED operation should be verified.                                      //
//                                                                                                                                      //
//  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License    //
//  as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.             //
//                                                                                                                                      //
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of      //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.                   //
//  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the              //
//  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA                                            //
//                                                                                                                                      //
//  It is not allowed to change any license or copyright statements, but feel free to modify, change, and add your own copyrights       //
//  below this line only !                                                                                                              //
//  ----------------------------------------------------------------------------------------------------------------------------------  //
//                                                                                                                                      //
//  Tested on AVR architecture (Arduino Nano) but should work with any architecture.                                                    //
//                                                                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
  void ledError([[maybe_unused]] uint8_t state,[[maybe_unused]] uint16_t adc) override { // Called when LED verification fails.
    _errCount++;                                    // Count LED verification failure.
  }
};

#endif