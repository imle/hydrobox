#include <dossing.h>
#include <constants/mqtt.h>
#include <mqtt_publish.h>
#include <network.h>


SenMLPack pack_dossing("urn:ctrl:");
SenMLStringRecord sml_state("dossing");

// 6: Last the reservoir will be flushed to the basin
bool dossingFlushReservoirToBasin(Task *me);
DelayRun th_flush_reservoir_to_basin(6000, dossingFlushReservoirToBasin);

// 5: After about 30 minutes, the pH should be balanced
bool dossingStopPhBalancing(Task *me);
DelayRun th_stop_ph_balancing(1800000, dossingStopPhBalancing, &th_flush_reservoir_to_basin);

// 4.2: Stop the pH dossing pumps after 1.333 seconds (~2ml)
bool dossingStopPhPump(Task *me);
DelayRun th_stop_ph_pump(1333, dossingStopPhPump);

// 4.1: Recurring task to trigger the pH dossing pumps
// Added when th_start_ph_balancing has been run and will be removed after th_stop_ph_balancing runs
void dossingTriggerBalanceBasinPh(Task *me);
Task th_balance_ph(20000, dossingTriggerBalanceBasinPh);

// 4: Start the pH balancing process after 2 minutes of letting the nutrients disperse
// not actually called by th_start_dossing, triggered by pumps finishing
bool dossingStartPhBalancing(Task *me);
DelayRun th_start_ph_balancing(120000, dossingStartPhBalancing, &th_stop_ph_balancing);

// 3.1: Added when th_start_dossing has been run and will be removed when all dossing pumps are finished
void dossingCheckState(Task *me);
Task th_check_dossing_state(10, dossingCheckState);

// 3: Begin dossing the reservoir with the General Hydroponics nutrients
bool dossingStart(Task *me);
DelayRun th_start_dossing(10000, dossingStart);

// 2: Stop the mixing fans
bool dossingStopMixNutrients(Task *me);
DelayRun th_stop_mix_nutrients(15000, dossingStopMixNutrients, &th_start_dossing);

// 1: Start the mixing fans
bool dossingStartMixNutrients(Task *me);
DelayRun th_start_mix_nutrients(0, dossingStartMixNutrients, &th_stop_mix_nutrients);

bool is_dosing = false;

void setupDossing() {
  pack_dossing.add(sml_state);
}

void triggerDossing() {
  if (!is_dosing) {
    is_dosing = true;

    th_start_mix_nutrients.startDelayed();
  }
}

bool isDossing() {
  return is_dosing;
}

bool dossingFlushReservoirToBasin(Task *me) {
  sml_state.set("flushing");
  printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

  // TODO: check amount of water in basin until empty
  is_dosing = false; // TODO: move this to the final call defined above
  return true;
}

bool dossingStopPhBalancing(Task *me) {
  sml_state.set("balancing_ph_finished");
  printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

  SoftTimer.remove(&th_balance_ph);
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
  sml_state.set("balancing_ph");
  printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

  SoftTimer.add(&th_balance_ph);
  return true;
}

void dossingCheckState(Task *me) {
  if (is_dosing && !pump_flora_micro.isOn() && !pump_flora_gro.isOn() && !pump_flora_bloom.isOn()) {
    sml_state.set("dossing_finished");
    printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

    SoftTimer.remove(&th_check_dossing_state);
    th_start_ph_balancing.startDelayed();
  }
}

bool dossingStart(Task *me) {
  sml_state.set("dossing");
  printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

  // TODO:
  SoftTimer.add(&th_check_dossing_state);
  return true;
}

bool dossingStopMixNutrients(Task *me) {
  sml_state.set("nutrient_stir_finished");
  printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

  mixers.off();

  return true;
}

bool dossingStartMixNutrients(Task *me) {
  sml_state.set("nutrient_stir_start");
  printAndPublish(MQTT_TOPIC_OUT_STATUS, pack_dossing);

  mixers.on();

  return true;
}