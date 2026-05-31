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