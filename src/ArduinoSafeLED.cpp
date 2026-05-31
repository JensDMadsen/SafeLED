//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  SafeLED - Arduino LED library with polarity detection and runtime verification.                                                     //
//  Copyright (C) 2026 Jens Dyekjær Madsen                                                                                              //
//                                                                                                                                      //
//  Filename: ArduinoSafeLED.cpp - Arduino GPIO and ADC backend implementation.                                                         //
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


#include "ArduinoSafeLED.h"

void ArduinoSafeLED::initHardware() {               // Initialize Arduino hardware backend.
  digitalWrite(_pinR,LOW);                          // Prepare output latch before enabling output driver.
  digitalWrite(_pinL,LOW);                          // Prepare output latch before enabling output driver.
  pinMode(_pinR,OUTPUT);                            // Enable output driver for _pinR.
  pinMode(_pinL,OUTPUT);                            // Enable output driver for _pinL.
  analogReference(DEFAULT);                         // Configure ADC reference.
}

void ArduinoSafeLED::driveOff() {                   // Disable LED current.
  digitalWrite(_pinR,LOW);                          // Drive _pinR low.
  digitalWrite(_pinL,LOW);                          // Drive _pinL low.
}

void ArduinoSafeLED::driveRL() {                    // Drive current from resistor side to LED side.
  digitalWrite(_pinR,HIGH);                         // Forward bias _pinR.
  digitalWrite(_pinL,LOW);                          // Sink current through _pinL.
}

void ArduinoSafeLED::driveLR() {                    // Drive current from LED side to resistor side.
  digitalWrite(_pinR,LOW);                          // Sink current through _pinR.
  digitalWrite(_pinL,HIGH);                         // Forward bias _pinL.
}

uint16_t ArduinoSafeLED::readAdc() {                // Returns normalized voltage between 0-65535.
  uint16_t adc = analogRead(_pinFb);                // Read 10-bit ADC value.
  return adc << 6;                                  // Normalize ADC value to 16-bit range.
}
