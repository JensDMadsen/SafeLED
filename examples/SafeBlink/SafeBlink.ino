#include "ArduinoSafeLED.h"

ArduinoSafeLED led1(16,A0,15);                  // WIRING: pinR --- resistor --- pinFb --- LED --- pinL

uint32_t speedMs = 250;                         // Set blink interval.
uint32_t nextMs = 0;                            // Next blink toggle time.
uint8_t  state  = 0;                            // Current blink state.

void setup() {
  Serial.begin(115200);                         // Initialize serial port.

  while (!Serial) {                             // Wait for native USB serial port.
  }

  Serial.println();                             // Print empty line.
  Serial.println("--- SafeLED Blink Test ---"); // Print test header.

  if (led1.init()) {                            // Initialize and detect LED.
    Serial.println("LED ok");                   // Report LED detected.
  }
  else {                                        // Otherwise initialization failed.
    Serial.println("LED error");                // Report LED error.
  }

  nextMs = millis() + speedMs;                  // Set first blink toggle time.
}

void loop() {
  if (!led1.check()) {                          // Verify LED continuously.
    Serial.println("--- LED error ---");        // Report verification error.

    if (led1.init()) {                          // Try to re-detect LED.
      Serial.println("--- LED restored ---");   // Report LED restored.
      led1.set(state);                          // Restore current blink state.
    }

    return;                                     // Keep loop running fast.
  }

  if ((int32_t)(millis() - nextMs) >= 0) {      // Check if toggle time has passed.
    nextMs += speedMs;                          // Schedule next fixed toggle time.

    if (state == 1) {                           // Check before possible state 2.
      if (!led1.init()) Serial.println("LED init error"); // Update LED type.
    }                                           // Allows change to state 2 if bi-color type.

    if (++state >= (led1.isBiColor()? 3 : 2)) { // Toggle state (0,1) or (0,1,2)
      state = 0;
    }

    Serial.println();                           // Insert empty line for space.
    if (led1.set(state)) Serial.println("LED ok"); else Serial.println("LED error");
    Serial.print("State:    "); Serial.println(state);
    Serial.print("Check:    "); Serial.println(led1.check());
    Serial.print("Colors:   "); Serial.println(led1.getColors());
    Serial.print("Reversed: "); Serial.println(led1.isReversed());
    Serial.print("Voltage:  "); Serial.println(led1.readVoltage()>>6);
  }
}