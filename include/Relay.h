#ifndef HPCC_RELAY_H
#define HPCC_RELAY_H

#include "Arduino.h"


class Relay {
 protected:
  pin_size_t pin;             // Output pin to turn on the relay
  PinStatus off_state;        // Default state of the controlling pin
  bool is_on{false};          // Current state of the relay
  bool on_async{false};       // Was the async activation timed
  unsigned long on_for_ms{0}; // How long the relay should be on. For use with checkShouldOff()
  unsigned long delay_on{0};  // How long the relay should be delayed for before async on is allowed to run
 public:
  Relay(pin_size_t pin, PinStatus off_state);

  void on();
  bool on(unsigned long ms);
  bool isOn() const;
  void off();
  bool checkShouldOff(); // For async (returns true if it turns off the relay)

  void delayNextOn(unsigned long ms);
  void cancelDelay();
};

#endif //HPCC_RELAY_H
