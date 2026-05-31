//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  SafeLED - Arduino LED library with polarity detection and runtime verification.                                                     //
//  Copyright (C) 2026 Jens Dyekjær Madsen                                                                                              //
//                                                                                                                                      //
//  Filename: ArduinoSafeLED.h   - Arduino GPIO and ADC backend implementation.                                                         //
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


#ifndef ARDUINOSAFELED_H
#define ARDUINOSAFELED_H

#include <Arduino.h>
#include "SafeLED.h"

class ArduinoSafeLED : public SafeLED {
protected:
  uint8_t _pinR;                                    // GPIO connected to resistor side.
  uint8_t _pinL;                                    // GPIO connected to LED side.
  uint8_t _pinFb;                                   // ADC feedback connection between resistor and LED.

public:                                             // WIRING: pinR --- resistor --- pinFb --- LED --- pinL
  ArduinoSafeLED(uint8_t pinR,uint8_t pinFb,uint8_t pinL): _pinR(pinR),_pinFb(pinFb),_pinL(pinL) { }

protected:
  void initHardware() override;                     // Initialize Arduino hardware backend.

  void driveOff() override;                         // Disable LED current.
  void driveRL() override;                          // Drive current from resistor side to LED side.
  void driveLR() override;                          // Drive current from LED side to resistor side.

  uint16_t readAdc() override;                      // Returns normalized voltage between 0-65535.
};

#endif