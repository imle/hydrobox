#ifndef HPCC_SUMP_H
#define HPCC_SUMP_H

#include <Task.h>


void checkPumpState(Task *me);
extern Task th_check_pump_state;

#endif //HPCC_SUMP_H
