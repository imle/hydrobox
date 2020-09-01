#ifndef HPCC_PUMP_H
#define HPCC_PUMP_H

#include <Arduino.h>
#include <PCA9685.h>


const double MilliliterToMilliseconds = 90.0 / (60.0 * 1000.0);

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
  PCA9685 *driver;           // If this is not null it will be used as the output driver
  int pin;                   // Output PWM pin to turn on the pump
  bool is_on{false};         // Current state of the pump
  uint16_t k;                // Value used for calibration (0 - 255)
  bool on_async{false};      // Has the pump been set on async
  unsigned long on_for_ms;   // How long the pump should be on. For use with checkShouldOff()
  unsigned long delay_on{0}; // How long the pump should be delayed for before async on is allowed to run
  bool calibrating;          // Is this pump calibrating
  bool locked_off;           // Off and stay off in case of leak or overflow
 public:
  Pump(int pin, int k);
  Pump(PCA9685 *driver, int pin, int k);

  State on();
  void off(bool emergency = false);

  State on(unsigned long ms);
  bool checkShouldOff();  // For async (returns true if it turns off the pump)

  State state() const;
  bool isOn() const;

  void delayNextOn(unsigned long ms);
  void cancelDelay();

  void beginCalibration();
  void setCalibrationKValue(uint16_t k);
  void endCalibration();
};

#endif //HPCC_PUMP_H
