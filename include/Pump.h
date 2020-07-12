#ifndef HPCC_PUMP_H
#define HPCC_PUMP_H

#include "Arduino.h"


const int KAddressEEPROMDefault = 8 * 16; // 128

class Pump {
 protected:
  pin_size_t pin;           // Output PWM pin to turn on the pump
  bool is_on{false};        // Current state of the pump
  byte k;                   // Value used for calibration (0 - 255)
  int k_address_eeprom;     // Address of k value in EEPROM
  unsigned long onForMS;    // How long the pump should be on. For use with checkShouldOff()
  unsigned long delayOn{0}; // How long the pump should be delayed for before async on is allowed to run
  bool calibrating;         // Is this pump calibrating
 public:
  Pump(pin_size_t pin);
  Pump(pin_size_t pin, int k_address_eeprom);

  void on();
  bool isOn() const;
  void off();

  bool on(unsigned long ms);
  bool checkShouldOff();  // For async (returns true if it turns off the pump)

  void delayNextOn(unsigned long ms);
  void cancelDelay();

  void onBlocking(unsigned long ms);

  void beginCalibration();
  void setCalibrationKValue(byte k);
  void endCalibration();
};

#endif //HPCC_PUMP_H
