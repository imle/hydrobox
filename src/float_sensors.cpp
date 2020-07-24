#include <float_sensors.h>


bool float_min_state = false;
Debouncer float_min_debouncer(FLOAT_MIN_PIN, MODE_OPEN_ON_PUSH, // On when down
                              []() { float_min_state = true; },
                              [](unsigned long) { float_min_state = false; },
                              true);

bool float_max_state = false;
Debouncer float_max_debouncer(FLOAT_MAX_PIN, MODE_CLOSE_ON_PUSH, // On when up
                              []() { float_max_state = true; },
                              [](unsigned long) { float_max_state = false; },
                              true);
