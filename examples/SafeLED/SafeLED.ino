#include "ArduinoSafeLED.h"

ArduinoSafeLED led1(16,A0,15); // WIRING: pinR --- resistor --- pinFb --- LED --- pinL

void setup() {
  //Initialize serial and wait for port to open (USB):
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();
  Serial.println("--- LED test ---");
  Serial.print("Init:     "); 
  if (led1.init()) Serial.println("LED ok"); else Serial.println("LED error");

  Serial.print("Colors:   "); Serial.println(led1.getColors());
  Serial.print("Reversed: "); Serial.println(led1.isReversed());
  Serial.print("Activate: "); Serial.println(led1.set(1));
  Serial.print("Voltage:  "); Serial.println(led1.readVoltage()>>6);
}

void loop() {
}