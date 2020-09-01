#ifndef HPCC_RELAY_H
#define HPCC_RELAY_H

#include <Arduino.h>


class Relay {
 protected:
  int pin;                    // Output pin to turn on the relay
  int off_state;              // Default state of the controlling pin
  bool is_on{false};          // Current state of the relay
 public:
  Relay(int pin, int off_state);

  void on();
  bool isOn() const;
  void off();
};

#endif //HPCC_RELAY_H
