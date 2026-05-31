# SafeLED

SafeLED is a library for verified mono and 2-pin bi-color status LEDs.

It does not just turn an LED on.
It also verifies that the LED behaves electrically as expected.

## Table of Contents

* [Introduction](#introduction)
* [Background and Motivation](#background-and-motivation)
* [Design Goals](#design-goals)
* [Hardware Concept](#hardware-concept)
* [LED Types](#led-types)
* [API Overview](#api-overview)
* [Runtime Verification](#runtime-verification)
* [Voltage Verification](#voltage-verification)
* [Watchdog Usage](#watchdog-usage)
* [Architecture](#architecture)
* [Arduino Backend Example](#arduino-backend-example)
* [Optional Statistics Layer](#optional-statistics-layer)
* [Embedded Fundamentals](#embedded-fundamentals)
* [Example Usage](#example-usage)
* [Industrial Motivation](#industrial-motivation)
* [License](#license)
* [Contact](#contact)

---

# Introduction

Most LED examples look like this:

```cpp
digitalWrite(LED_PIN, HIGH);    // Turn LED on.
```

This assumes that:

* the LED is mounted correctly
* the solder joints are good
* the connector is connected
* the LED is not damaged
* the polarity is correct
* the signal path works

In real hardware, this is not always true.

SafeLED adds electrical feedback and runtime verification to simple status LEDs.

---

# Background and Motivation

SafeLED was created from the idea that even simple outputs should sometimes be verified.

An LED may be used as:

* a status indicator
* an alarm indicator
* a warning lamp
* a brake light
* a service indicator
* or another visual signal

In such cases, it may not be enough for software to simply write to an output pin.

SafeLED can help detect:

* missing LEDs
* reversed LEDs
* incorrect polarity
* unstable connections
* bad solder joints
* intermittent failures
* connector problems
* unexpected LED voltage

The goal is not only to control LEDs.

The goal is to verify that reality matches software assumptions.

---

# Design Goals

SafeLED is designed to be:

* simple to use
* suitable for mono LEDs
* suitable for 2-pin bi-color LEDs
* able to detect LED polarity
* able to correct reversed LED mounting in software
* able to verify LED voltage
* usable during runtime
* suitable for embedded diagnostics
* portable to different hardware platforms

SafeLED is intended for one status LED or one 2-pin bi-color LED, not LED strips, LED matrices, RGB displays, or animation effects.

---

# Hardware Concept

SafeLED uses two GPIO pins, one current limiting resistor, one LED, and one ADC input.

```text
LED_R  >──┐
          │
         ┌┴┐
         │R│
         │ │
         └┬┘
          │
LED_FB <──┤
          │
         ─┴─ /
         \ / - LED
         ─┬─ \
          │
LED_L  >──┘
```

`LED_R` and `LED_L` are driven as outputs.

`LED_FB` is connected to an ADC input and measures the voltage at the junction between the resistor and the LED.

Current can flow in both directions:

```text
LED_R → R → LED → LED_L
LED_L → LED → R → LED_R
```

This allows the library to detect which direction conducts.

For a mono LED, only one direction should conduct.

For a 2-pin bi-color LED, both directions should conduct, normally with different forward voltages.

---

# LED Types

SafeLED detects the number of available LED channels during initialization.

| Value | Meaning                     |
| ----- | --------------------------- |
| 0     | No LED detected             |
| 1     | Mono LED detected           |
| 2     | 2-pin bi-color LED detected |

Internally this may be represented as:

```cpp
uint8_t _colors;        // detected LED capability, set by init()
bool    _reversed;      // detected physical polarity correction, set by init()
uint8_t _state;         // current logical LED state, set by set()
```

Where:

```text
_colors:
  0 = no LED
  1 = mono LED
  2 = bi-color LED

_reversed:
  false = physical LED polarity matches standard direction
  true  = physical LED polarity is logically reversed

_state:
  0 = LED turned off
  1 = Mono LED or low-Vf channel of a bi-color LED turned on
  2 = High-Vf channel of a bi-color LED turned on
```

---

# API Overview

## Initialization

```cpp
bool init(uint8_t requiredColors = 0);  // Initialize and detect LED type.
```

| Value | Meaning                     |
| ----- | --------------------------- |
| 0     | Accept mono or bi-color LED |
| 1     | Require mono LED            |
| 2     | Require bi-color LED        |

Returns:

```text
true  = required LED type detected and validated
false = no LED, wrong LED type, or invalid voltage
```

`init()` detects:

* whether an LED is present
* whether it is mono or bi-color
* which physical direction conducts
* whether polarity must be reversed internally

## Set LED State

```cpp
bool set(uint8_t state);        // Set logical LED state.
```

| State | Meaning                           |
| ----- | --------------------------------- |
| 0     | Off                               |
| 1     | Mono LED or low-Vf channel        |
| 2     | High-Vf channel of a bi-color LED |

Example validity check:

```cpp
if ((state > _colors)||(_colors == 0)) {    // Check if LED is detected and requested state is supported.
  ledError(state,0);                        // Report unsupported LED state.
  return false;                             // Reject unsupported LED state.
}
```

If `_colors` is `0`, all attempts to turn on an LED fail.

## Query LED Type

```cpp
uint8_t getColors() const;      // Returns detected LED capability.
bool isMono() const;            // Returns true if a mono LED is detected.
bool isBiColor() const;         // Returns true if a bi-color LED is detected.
```

These helper functions allow the application to query the detected LED capability after `init()`.

`getColors()` returns the internally detected LED capability value:

| Value | Meaning                     |
| ----- | --------------------------- |
| 0     | No LED detected             |
| 1     | Mono LED detected           |
| 2     | 2-pin bi-color LED detected |

Typical usage:

```cpp
if (led.isBiColor()) {          // Check if LED supports second color.
    led.set(2);                 // Turn on high-Vf LED channel.
}
```

Implementations:

```cpp
uint8_t getColors() const {     // Returns detected LED capability.
    return _colors;             // Return detected LED capability.
}

bool isMono() const {           // Returns true if a mono LED is detected.
    return _colors == 1;        // Check if detected LED is mono.
}

bool isBiColor() const {        // Returns true if a bi-color LED is detected.
    return _colors == 2;        // Check if detected LED is bi-color.
}
```

These helper functions are optional convenience wrappers around `_colors`.

Applications may also use `getColors()` directly if more compact code is preferred.

---

# Runtime Verification

```cpp
bool check();   // Verify LED operation during runtime.
```

`check()` performs a non-intrusive verification:

* if the LED is off, no electrical LED test is performed
* if the LED is on, the LED voltage is measured and validated

This avoids unwanted visible blinking during runtime diagnostics.

---

# Voltage Verification

SafeLED works with normalized ADC values, not floating point voltages.

A typical internal ADC type may be:

```cpp
using LedAdc = uint16_t;                // Normalized ADC type.
```

Applications may override:

```cpp
virtual bool voltageOk1(uint16_t adc);  // Validate mono LED or low-Vf channel.
virtual bool voltageOk2(uint16_t adc);  // Validate high-Vf bi-color channel.
```

Default implementation example:

```cpp
virtual bool voltageOk1(uint16_t adc) { // Validate mono LED or low-Vf channel.
    return adc >= minDefault1 &&
           adc <= maxDefault1;          // Check upper and lower voltage limits.
}

virtual bool voltageOk2(uint16_t adc) { // Validate high-Vf bi-color channel.
    return adc >= minDefault2 &&
           adc <= maxDefault2;          // Check upper and lower voltage limits.
}
```

Applications may override these functions to implement application-specific validation.

Example:

```cpp
class MySafeLED : public ArduinoSafeLED {
public:
    using ArduinoSafeLED::ArduinoSafeLED;       // Inherit constructor(s) from ArduinoSafeLED.
protected:
    bool voltageOk1(uint16_t adc) override {    // Validate red LED channel.
        return adc >= 12000 &&
               adc <= 32000;                    // Check upper and lower voltage limits.
    }

    bool voltageOk2(uint16_t adc) override {    // Validate green LED channel.
        return adc >= 22000 &&
               adc <= 52000;                    // Check upper and lower voltage limits.
    }
};
```

This allows the application to:

* validate specific LED types
* validate supply voltage assumptions
* detect unexpected hardware behavior
* tighten diagnostic limits for safety-critical systems

---

# Watchdog Usage

A watchdog should only be cleared when the system is known to operate correctly.

In some systems, LEDs are used as important status indicators. If such an indicator fails, the system may no longer provide valid visual status information.

SafeLED makes it possible to verify that the LED is electrically plausible before clearing the watchdog.

Example:

```cpp
if (statusLed.check()) {        // Verify LED operation.
    wdt_reset();                // Clear watchdog timer.
}
```

The `check()` function does not turn the LED on by itself. It only verifies the LED if it is already on.

If LED verification fails, the application may:

* avoid clearing the watchdog
* enter a safe state
* log the fault
* signal an error through other mechanisms

---

# Architecture

SafeLED can be implemented as a small hardware-independent core.

The core state can be as small as:

```cpp
uint8_t _colors;    // detected LED capability, set by init()
bool    _reversed;  // detected physical polarity correction, set by init()
uint8_t _state;     // current logical LED state, set by set()
```

Hardware access can be supplied through virtual functions:

```cpp
virtual void initHardware() = 0;                    // Initialize hardware backend.

virtual void driveOff() = 0;                        // Disable LED current.
virtual void driveRL()  = 0;                        // Drive current from resistor side to LED side.
virtual void driveLR()  = 0;                        // Drive current from LED side to resistor side.

virtual uint16_t readAdc() = 0;                     // Returns normalized voltage between 0-65535.

virtual void ledError(uint8_t state, uint16_t adc); // Called when LED verification fails.
```

The hardware backend only controls the physical direction.

SafeLED uses normalized 16-bit ADC values.

Independent of the physical ADC resolution, all ADC measurements are normalized to the range 0-65535.

Examples:

10-bit ADC:
  adc16 = adc10 << 6

12-bit ADC:
  adc16 = adc12 << 4

16-bit ADC:
  adc16 = adc16


The SafeLED core maps logical states such as low-Vf and high-Vf to the physical RL/LR directions after `init()` has detected the LED.

This allows the same core logic to be used with:

* Arduino GPIO
* direct port access
* template-based pin backends
* other microcontrollers
* unit test backends

---

# Arduino Backend Example

```cpp
class ArduinoSafeLED : public SafeLED {
protected:
    uint8_t _pinR;                          // GPIO connected to resistor side.
    uint8_t _pinL;                          // GPIO connected to LED side.
    uint8_t _pinFb;                         // ADC feedback connection between resistor and LED.

public:                                     // WIRING: pinR --- resistor --- pinFb --- LED --- pinL
    ArduinoSafeLED(uint8_t pinR,uint8_t pinFb,uint8_t pinL): _pinR(pinR),_pinFb(pinFb),_pinL(pinL) { }

protected:
    void initHardware() override {          // Initialize Arduino hardware backend.
        digitalWrite(_pinR, LOW);           // Prepare output latch before enabling output driver.
        digitalWrite(_pinL, LOW);           // Prepare output latch before enabling output driver.
        pinMode(_pinR, OUTPUT);             // Enable output driver for _pinR.
        pinMode(_pinL, OUTPUT);             // Enable output driver for _pinL.
        analogReference(DEFAULT);           // Configure ADC reference.
    }

    void driveOff() override {              // Disable LED current.
        digitalWrite(_pinR, LOW);           // Drive _pinR low.
        digitalWrite(_pinL, LOW);           // Drive _pinL low.
    }

    void driveRL() override {               // Drive current from resistor side to LED side.
        digitalWrite(_pinR, HIGH);          // Forward bias _pinR.
        digitalWrite(_pinL, LOW);           // Sink current through _pinL.
    }

    void driveLR() override {               // Drive current from LED side to resistor side.
        digitalWrite(_pinR, LOW);           // Sink current through _pinR.
        digitalWrite(_pinL, HIGH);          // Forward bias _pinL.
    }

    uint16_t readAdc() override {           // Returns normalized voltage between 0-65535.
        uint16_t adc = analogRead(_pinFb);  // Read 10-bit ADC value.
        return adc << 6;                    // Normalize ADC value to 16-bit range.
    }
};
```

More advanced implementations may use:

* direct port access
* templates
* compile-time pin mapping
* hardware abstraction layers
* optimized GPIO libraries

to reduce code size and increase performance.

---

# Optional Statistics Layer

SafeLED supports optional diagnostic and statistics layers through the `ledError()` hook.

The default implementation may be empty:

```cpp
virtual void ledError(uint8_t state, uint16_t adc) { }  // Called when LED verification fails.
```

Applications may implement additional diagnostics by overriding this hook.

Example:

```cpp
class ArduinoSafeLEDstats : public ArduinoSafeLED {
protected:
    uint32_t _errCount = 0;                                 // LED verification error counter.
public:
    using ArduinoSafeLED::ArduinoSafeLED;                   // Inherit constructor(s) from ArduinoSafeLED.

    uint32_t errCount() const {                             // Returns LED verification error counter.
        return _errCount;                                   // Return LED verification error counter.
    }

    void clearErrCount() {                                  // Clears LED verification error counter.
        _errCount = 0;                                      // Clear LED verification error counter.
    }
protected:
    void ledError(uint8_t state, uint16_t adc) override {   // Called when LED verification fails.
        _errCount++;                                        // Count LED verification failure.
    }
};
```

This demonstrates how:

* diagnostics can be layered on top of the core library
* applications only pay RAM and code size for features they use
* more advanced logging or statistics systems may be implemented later

---

# Embedded Fundamentals

SafeLED is intentionally simple.

The purpose is not only to control an LED, but also to demonstrate important embedded fundamentals:

* outputs should sometimes be verified
* hardware is not always ideal
* runtime diagnostics matter
* watchdogs should only be cleared when the system behaves correctly
* intermittent faults exist
* software assumptions should be validated against reality

---

# Example Usage

```cpp
if (led.init()) {                       // Initialize and detect LED type.
    led.set(1);                         // Turn on mono or low-Vf LED channel.
    delay(500);                         // Wait 500 milliseconds.
    if (led.isBiColor()) {              // Check if LED supports second color.
        led.set(2);                     // Turn on high-Vf LED channel.
    }
    delay(500);                         // Wait 500 milliseconds.
    led.set(0);                         // Turn LED off.
}
```

Watchdog example:

```cpp
led.set(1);                             // Turn status LED on.
if (led.check()) {                      // Verify LED operation.
    wdt_reset();                        // Clear watchdog timer.
}
```

Statistics example:

```cpp
                                        // WIRING: pinR --- resistor --- pinFb --- LED --- pinL
ArduinoSafeLEDstats led(16,A0,15);      // Create LED with statistics layer.
led.clearErrCount();                    // Clear previous diagnostic history.
if (!led.init()) {                      // Initialize and detect LED.
    Serial.println(                     // Print LED verification error count.
        led.errCount()
    );
}
```

---

# Industrial Motivation

SafeLED was inspired by real production and industrial test environments where:

* LEDs may be mounted reversed
* hand-soldered LEDs may have polarity errors
* front panel indicators may be connected through cables
* connectors may become unstable
* products may run for weeks or months without reboot
* intermittent faults matter
* visual indicators may be part of system diagnostics

Sometimes the simplest components deserve verification too.

---

# License

SafeLED is licensed under the LGPL-2.1 license.

---

## Contact

If you have any questions, feedback, or need further assistance, feel free to [Contact Me](https://www.arduino.one/contact.html) through my online form.
