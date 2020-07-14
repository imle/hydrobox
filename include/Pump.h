#ifndef HPCC_PUMP_H
#define HPCC_PUMP_H

#include "Arduino.h"
#include "Adafruit_PWMServoDriver.h"


const int KAddressEEPROMDefault = 8 * 16; // 128

class Pump {
 public:
  enum State {
    OFF,
    ON,
    TOGGLED_ON,
    LOCKED_OFF,
    DELAYED_OFF,
  };
 protected:
  Adafruit_PWMServoDriver *driver; // If this is not null it will be used as the output driver
  pin_size_t pin;                  // Output PWM pin to turn on the pump
  bool is_on{false};               // Current state of the pump
  uint16_t k;                      // Value used for calibration (0 - 255)
  int k_address_eeprom;            // Address of k value in EEPROM
  unsigned long onForMS;           // How long the pump should be on. For use with checkShouldOff()
  unsigned long delayOn{0};        // How long the pump should be delayed for before async on is allowed to run
  bool calibrating;                // Is this pump calibrating
  bool locked_off;                 // Off and stay off in case of leak or overflow
 public:
  Pump(pin_size_t pin);
  Pump(pin_size_t pin, int k_address_eeprom);
  Pump(Adafruit_PWMServoDriver *driver, pin_size_t pin);
  Pump(Adafruit_PWMServoDriver *driver, pin_size_t pin, int k_address_eeprom);

  State on();
  State state() const;
  void off(bool emergency = false);

  State on(unsigned long ms);
  bool checkShouldOff();  // For async (returns true if it turns off the pump)

  void delayNextOn(unsigned long ms);
  void cancelDelay();

  bool onBlocking(unsigned long ms);

  void beginCalibration();
  void setCalibrationKValue(uint16_t k);
  void endCalibration();
};

#endif //HPCC_PUMP_H
