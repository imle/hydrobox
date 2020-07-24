#ifndef HPCC_BUTTON_ACTIONS_H
#define HPCC_BUTTON_ACTIONS_H

#include <Debouncer.h>
#include <Pump.h>

//#define BUTTON_HOLD_ON_MIXERS
//#define BUTTON_HOLD_ON_PUMPS
//#define BUTTON_TOGGLE_ON
//#define BUTTON_TOGGLE_CALIBRATOR
#define BUTTON_START_DOSE

extern Pump *button_pump;

#define BUTTON_PIN 2
extern Debouncer button_debouncer;

void buttonPressed();

void buttonReleased(unsigned long pressTimespan);

#endif //HPCC_BUTTON_ACTIONS_H
