#ifndef HPCC_PUMP_H
#define HPCC_PUMP_H

#include <Arduino.h>
#include <PCA9685.h>


const double MilliliterToMilliseconds = 90.0 / (60.0 * 1000.0);

class Pump {
 public:
 protected:
  PCA9685 *driver;           // If this is not null it will be used as the output driver
  int pin;                   // Output PWM pin to turn on the pump
  bool is_on{false};         // Current state of the pump
  uint16_t k;                // Value used for calibration (0 - 255)
 public:
  Pump(int pin, int k);
  Pump(PCA9685 *driver, int pin, int k);

  void on();
  bool isOn() const;
  void off();
};

#endif //HPCC_PUMP_H
