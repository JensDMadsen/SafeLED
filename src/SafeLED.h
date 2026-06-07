//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  SafeLED - Arduino LED library with polarity detection and runtime verification.                                                     //
//  Copyright (C) 2026 Jens Dyekjær Madsen                                                                                              //
//                                                                                                                                      //
//  Filename: SafeLED.h   - Core LED detection and runtime verification logic.                                                          //
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


#ifndef SAFELED_H
#define SAFELED_H

#include <stdint.h>

class SafeLED {
protected:
  static constexpr uint16_t minDefault1 = 0x1000;       // Default minimum mono/low-Vf voltage.
  static constexpr uint16_t maxDefault1 = 0xF000;       // Default maximum mono/low-Vf voltage.

  static constexpr uint16_t minDefault2 = 0x1000;       // Default minimum high-Vf voltage.
  static constexpr uint16_t maxDefault2 = 0xF000;       // Default maximum high-Vf voltage.

  uint8_t _colors   = 0;                                // Detected LED capability, set by init().
  bool    _reversed = false;                            // Detected physical polarity correction, set by init().
  uint8_t _state    = 0;                                // Current logical LED state, set by set().

public:
  bool init(uint8_t requiredColors = 0);                // Initialize and detect LED type.

  bool set(uint8_t state);                              // Set logical LED state.

  bool check();                                         // Verify LED operation during runtime.

  uint16_t readVoltage() { return applyState(_state); } // Re-measure current LED voltage.

  uint8_t getColors() const { return _colors;         } // Returns detected LED capability.
  bool isMono() const       { return _colors == 1;    } // Returns true if a mono LED is detected.
  bool isBiColor() const    { return _colors == 2;    } // Returns true if a bi-color LED is detected.
  bool isReversed() const   { return _reversed;       } // Returns true if LED polarity is reversed.

protected:
  virtual void initHardware() = 0;                      // Initialize hardware backend.

  virtual void driveOff() = 0;                          // Disable LED current.
  virtual void driveRL()  = 0;                          // Drive current from resistor side to LED side.
  virtual void driveLR()  = 0;                          // Drive current from LED side to resistor side.

  virtual uint16_t readAdc() = 0;                       // Returns normalized voltage between 0-65535.

  virtual bool voltageOk1(uint16_t adc) {               // Validate mono LED or low-Vf channel.
    return adc >= minDefault1 && adc <= maxDefault1;    // Check upper and lower voltage limits.
  }

  virtual bool voltageOk2(uint16_t adc) {               // Validate high-Vf bi-color channel.
    return adc >= minDefault2 && adc <= maxDefault2;    // Check upper and lower voltage limits.
  }

  virtual void ledError([[maybe_unused]] uint8_t state, [[maybe_unused]] uint16_t adc) { } // Called when LED verification fails.

private:
  bool validateState(uint8_t state, uint16_t adc);      // Validate voltage for logical LED state.
  uint16_t applyState(uint8_t state);                   // Apply logical LED state and return voltage.
};

#endif
