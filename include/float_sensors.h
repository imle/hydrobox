#ifndef HPCC_FLOAT_SENSORS_H
#define HPCC_FLOAT_SENSORS_H

#include <Debouncer.h>


#define FLOAT_MIN_PIN 7
#define FLOAT_MAX_PIN 8

extern bool float_min_state;
extern bool float_max_state;

extern Debouncer float_min_debouncer;
extern Debouncer float_max_debouncer;

#endif //HPCC_FLOAT_SENSORS_H
