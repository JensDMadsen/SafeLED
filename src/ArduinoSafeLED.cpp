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
