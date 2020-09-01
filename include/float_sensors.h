#ifndef HPCC_FLOAT_SENSORS_H
#define HPCC_FLOAT_SENSORS_H

#include <Debouncer.h>

extern bool float_rails_state;
extern bool float_min_state;
extern bool float_max_state;

extern Debouncer float_rails_debouncer;
extern Debouncer float_min_debouncer;
extern Debouncer float_max_debouncer;

void setupFloatSensors();

#endif //HPCC_FLOAT_SENSORS_H
