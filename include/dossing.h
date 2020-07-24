#ifndef HPCC_DOSSING_H
#define HPCC_DOSSING_H

#include <Task.h>
#include <DelayRun.h>
#include <tools.h>


// 6: Last the reservoir will be flushed to the basin
bool dossingFlushReservoirToBasin(Task *me);
extern DelayRun th_flush_reservoir_to_basin;

// 5: After about 30 minutes, the pH should be balanced
bool dossingStopPhBalancing(Task *me);
extern DelayRun th_stop_ph_balancing;

// 4.2: Stop the pH dossing pumps after 1.333 seconds (~2ml)
bool dossingStopPhPump(Task *me);
extern DelayRun th_stop_ph_pump;

// 4.1: Recurring task to trigger the pH dossing pumps
// Added when th_start_ph_balancing has been run and will be removed after th_stop_ph_balancing runs
void dossingTriggerBalanceBasinPh(Task *me);
extern Task th_balance_ph;

// 4: Start the pH balancing process after 2 minutes of letting the nutrients disperse
// not actually called by th_start_dossing, triggered by pumps finishing
bool dossingStartPhBalancing(Task *me);
extern DelayRun th_start_ph_balancing;

// 3.1: Added when th_start_dossing has been run and will be removed when all dossing pumps are finished
void dossingCheckState(Task *me);
extern Task th_check_dossing_state;

// 3: Begin dossing the reservoir with the General Hydroponics nutrients
bool dossingStart(Task *me);
extern DelayRun th_start_dossing;

// 2: Stop the mixing fans
bool dossingStopMixNutrients(Task *me);
extern DelayRun th_stop_mix_nutrients;

// 1: Start the mixing fans
bool dossingStartMixNutrients(Task *me);
extern DelayRun th_start_mix_nutrients;

extern const int mixer_count;
extern Relay *mixers[];
extern bool is_dosing;

#endif //HPCC_DOSSING_H
