#include <dossing.h>


DelayRun th_flush_reservoir_to_basin(6000, dossingFlushReservoirToBasin);
DelayRun th_stop_ph_balancing(1800000, dossingStopPhBalancing, &th_flush_reservoir_to_basin);
DelayRun th_stop_ph_pump(1333, dossingStopPhPump);
Task th_balance_ph(20000, dossingTriggerBalanceBasinPh);
DelayRun th_start_ph_balancing(120000, dossingStartPhBalancing, &th_stop_ph_balancing);
Task th_check_dossing_state(10, dossingCheckState);
DelayRun th_start_dossing(10000, dossingStart);
DelayRun th_stop_mix_nutrients(15000, dossingStopMixNutrients, &th_start_dossing);
DelayRun th_start_mix_nutrients(0, dossingStartMixNutrients, &th_stop_mix_nutrients);

const int mixer_count = 2;
Relay *mixers[] = {&fan0, &fan1};
bool is_dosing = false;

bool dossingStopPhBalancing(Task *me) {
  Serial.println("balancing ph finished");
  SoftTimer.remove(&th_balance_ph);
  return true;
}

bool dossingFlushReservoirToBasin(Task *me) {
  Serial.println("flushing");
  // TODO: check amount of water in basin until empty
  is_dosing = false; // TODO: move this to the final call defined above
  return true;
}

bool dossingStopPhPump(Task *me) {
  Serial.println("  ph pumps off");
  return true;
}

void dossingTriggerBalanceBasinPh(Task *me) {
  Serial.println("balancing ph");
  Serial.println("  ph pumps on");

  th_stop_ph_pump.startDelayed();
}

bool dossingStartPhBalancing(Task *me) {
  Serial.println("balancing ph starting");
  SoftTimer.add(&th_balance_ph);
  return true;
}

void dossingCheckState(Task *me) {
  if (is_dosing && !pump_flora_micro.isOn() && !pump_flora_gro.isOn() && !pump_flora_bloom.isOn()) {
    Serial.println("finished dossing");
    SoftTimer.remove(&th_check_dossing_state);
    th_start_ph_balancing.startDelayed();
  }
}

bool dossingStart(Task *me) {
  Serial.println("dossing");
  // TODO:
  SoftTimer.add(&th_check_dossing_state);
  return true;
}

bool dossingStopMixNutrients(Task *me) {
  Serial.println("stop mixing");
  for (auto &mixer : mixers) {
    mixer->off();
  }
  return true;
}

bool dossingStartMixNutrients(Task *me) {
  Serial.println("start mixing");
  for (auto &mixer : mixers) {
    mixer->on();
  }
  return true;
}