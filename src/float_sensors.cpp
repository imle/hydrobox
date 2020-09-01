#include <float_sensors.h>
#include <pin_assignment.h>
#include <debug.h>


bool float_rails_state = false;
Debouncer float_rails_debouncer(PIN_FLOAT_RAILS, MODE_OPEN_ON_PUSH, // On when open
                                []() { float_rails_state = true; },
                                [](unsigned long) { float_rails_state = false; },
                                true);

bool float_min_state = false;
Debouncer float_min_debouncer(PIN_FLOAT_MIN, MODE_OPEN_ON_PUSH, // On when down
                              []() { float_min_state = true; },
                              [](unsigned long) { float_min_state = false; },
                              true);

bool float_max_state = false;
Debouncer float_max_debouncer(PIN_FLOAT_MAX, MODE_CLOSE_ON_PUSH, // On when up
                              []() { float_max_state = true; },
                              [](unsigned long) { float_max_state = false; },
                              true);

void setupFloatSensors() {
  FUNC_IN

  float_rails_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT_RAILS), []() { float_rails_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_min_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT_MIN), []() { float_min_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_max_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT_MAX), []() { float_max_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  FUNC_OUT
}
