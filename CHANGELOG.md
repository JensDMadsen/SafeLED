# Changelog

All notable changes to this project will be documented in this file.

## 1.0.1 - 2026-05-31
### Changed
- Added source file headers with copyright, license, and file descriptions.
- No API or functional changes.

## [1.0.0] - 2026-05-31
### Initial Release
Features:
- Mono LED support
- 2-pin bi-color LED support
- Automatic polarity detection
- Runtime voltage verification
- Reversed LED correction
- Optional statistics layer
- Arduino backend
- SafeLED and SafeBlink examples

### Requirements
- LED must be connected
- Uses 3-pins, two digital outputs, one analog input
- WIRING: pinR(OUT) --- resistor --- pinFb(ADC IN) --- LED --- pinL(OUT)
