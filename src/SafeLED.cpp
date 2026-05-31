#include "SafeLED.h"

bool SafeLED::init(uint8_t requiredColors) {            // Initialize and detect LED type.
  initHardware();                                       // Initialize hardware backend.

  _reversed = false;                                    // Assume standard polarity during detection.
  uint16_t adc1 = applyState(1);                        // Measure LED voltage, current R to L.
  uint16_t adc2 = applyState(2);                        // Measure LED voltage, current L to R.
  applyState(0);                                        // Disable LED after detection.

  _reversed = adc1 > adc2;                              // Is reversed if adc1 is highest.
  if (_reversed) {                                      // Store voltages in adc1 (low-Vf) and adc2 (high-Vf).
    uint16_t temp = adc1;                               // Swap adc1 and adc2 values.
    adc1 = adc2;                                        // Store low-Vf voltage.
    adc2 = temp;                                        // Store high-Vf voltage.
  }

  bool ok1 = voltageOk1(adc1);                          // Validate mono or low-Vf channel.
  bool ok2 = voltageOk2(adc2);                          // Validate high-Vf channel.

  if (!ok1) {                                           // If mono or low-Vf not ok:
    _colors = 0;                                        // Store no LED detected.
    ledError(0,0);                                      // Report LED detection failure.
    return false;                                       // Initialization failed.
  }

  _colors = ok2 ? 2 : 1;                                // Store detected LED capability.

  if (requiredColors && (requiredColors != _colors)) {  // Check if detected type matches requirement.
    ledError(_colors,0);                                // Report wrong LED type.
    return false;                                       // Initialization failed.
  }

  return true;                                          // Initialization succeeded.
}

bool SafeLED::set(uint8_t state) {                      // Set logical LED state.
  if ((state > _colors)||(_colors == 0)) {              // Check if LED is detected and requested state is supported.
    ledError(state,0);                                  // Report unsupported LED state.
    return false;                                       // Reject unsupported LED state.
  }

  uint16_t adc = applyState(state);                     // Apply state and read LED voltage.

  if (!validateState(state,adc)) {                      // Validate measured LED voltage.
    ledError(state,adc);                                // Report LED verification failure.
    return false;                                       // Verification failed.
  }

  return true;                                          // LED state accepted and verified.
}

bool SafeLED::check() {                                 // Verify LED operation during runtime.
  return set(_state);                                   // Re-apply and verify current LED state.
}

bool SafeLED::validateState(uint8_t state,uint16_t adc) {   // Validate voltage for logical LED state.
  if (state == 1) {                                     // Check mono or low-Vf channel.
    return voltageOk1(adc);                             // Validate state 1 voltage.
  }

  if (state == 2) {                                     // Check high-Vf channel.
    return voltageOk2(adc);                             // Validate state 2 voltage.
  }

  return state == 0;                                    // Only off state is valid without voltage.
}

uint16_t SafeLED::applyState(uint8_t state) {           // Apply logical LED state and return voltage.
  _state = state;                                       // Store current logical LED state.

  if (state == 0) {                                     // Check if LED should be off.
    driveOff();                                         // Disable LED current.
    return 0;                                           // Return off-state voltage.
  }

  if ((state == 1) != _reversed) {                      // Map logical state to physical RL direction.
    driveRL();                                          // Drive LED in RL direction.
    return readAdc();                                   // Return normalized LED voltage.
  }
  else {                                                // Otherwise use physical LR direction.
    driveLR();                                          // Drive LED in LR direction.
    return 65535 - readAdc();                           // Return inverted normalized LED voltage.
  }
}
