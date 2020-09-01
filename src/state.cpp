#include <state.h>

#include <PhysicalStates.h>
#include <float_sensors.h>
#include <network.h>
#include <tools.h>


SenMLPack state("urn:ctrl:state:");
SenMLBoolRecord sml_pumps_locked_off("pumps_locked_off", SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_micro("pump_flora_micro", SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_gro("pump_flora_gro", SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_bloom("pump_flora_bloom", SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_up_reservoir("pump_ph_up_reservoir", SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_up_basin("pump_ph_up_basin", SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_down_basin("pump_ph_down_basin", SENML_UNIT_RATIO);
SenMLBoolRecord sml_submersible_pump("submersible_pump", SENML_UNIT_RATIO);
SenMLBoolRecord sml_plant_lights("plant_lights", SENML_UNIT_RATIO);
SenMLBoolRecord sml_air_mover("air_mover", SENML_UNIT_RATIO);
SenMLBoolRecord sml_bubbler("bubbler", SENML_UNIT_RATIO);
SenMLBoolRecord sml_r4("r4", SENML_UNIT_RATIO);
SenMLBoolRecord sml_r5("r5", SENML_UNIT_RATIO);
SenMLBoolRecord sml_r6("r6", SENML_UNIT_RATIO);
SenMLBoolRecord sml_r7("r7", SENML_UNIT_RATIO);
SenMLBoolRecord sml_fan0("fan0", SENML_UNIT_RATIO);
SenMLBoolRecord sml_fan1("fan1", SENML_UNIT_RATIO);
SenMLBoolRecord sml_rail_valves("rail_valves", SENML_UNIT_RATIO);
SenMLBoolRecord sml_float_rails("float_rails", SENML_UNIT_RATIO);
SenMLBoolRecord sml_float_min("float_min", SENML_UNIT_RATIO);
SenMLBoolRecord sml_float_max("float_max", SENML_UNIT_RATIO);

Task th_check_state_changes_and_notify(100, checkStateChangesAndSendUpdateMessageIfNecessary);

PhysicalStates last_states;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-non-const-parameter"

void setLastState(SenMLPack &pack, SenMLBoolRecord &record, const bool current_state, bool &state_storage) {
  if (state_storage != current_state) {
    pack.add(record);
    record.set(current_state);
    state_storage = current_state;
  }
}

void checkStateChangesAndSendUpdateMessageIfNecessary(Task *me) {
  state.clear();

  // If any one pump is locked off, all of them are
  setLastState(state, sml_pumps_locked_off, pump_flora_micro.state() == Pump::State::LOCKED_OFF, last_states.pumps_locked_off);

  setLastState(state, sml_pump_flora_micro, pump_flora_micro.isOn(), last_states.pump_flora_micro);
  setLastState(state, sml_pump_flora_gro, pump_flora_gro.isOn(), last_states.pump_flora_gro);
  setLastState(state, sml_pump_flora_bloom, pump_flora_bloom.isOn(), last_states.pump_flora_bloom);
  setLastState(state, sml_pump_ph_up_reservoir, pump_ph_up_reservoir.isOn(), last_states.pump_ph_up_reservoir);
  setLastState(state, sml_pump_ph_up_basin, pump_ph_up_basin.isOn(), last_states.pump_ph_up_basin);
  setLastState(state, sml_pump_ph_down_basin, pump_ph_down_basin.isOn(), last_states.pump_ph_down_basin);
  setLastState(state, sml_submersible_pump, submersible_pump.isOn(), last_states.submersible_pump);
  setLastState(state, sml_plant_lights, plant_lights.isOn(), last_states.plant_lights);
  setLastState(state, sml_air_mover, air_mover.isOn(), last_states.air_mover);
  setLastState(state, sml_bubbler, bubbler.isOn(), last_states.bubbler);
  setLastState(state, sml_r4, r4.isOn(), last_states.r4);
  setLastState(state, sml_r5, r5.isOn(), last_states.r5);
  setLastState(state, sml_r6, r6.isOn(), last_states.r6);
  setLastState(state, sml_r7, r7.isOn(), last_states.r7);
  setLastState(state, sml_fan0, fan0.isOn(), last_states.fan0);
  setLastState(state, sml_fan1, fan1.isOn(), last_states.fan1);
  setLastState(state, sml_rail_valves, rail_valves.isOn(), last_states.rail_valves);
  setLastState(state, sml_float_rails, float_rails_state, last_states.float_min);
  setLastState(state, sml_float_min, float_min_state, last_states.float_min);
  setLastState(state, sml_float_max, float_max_state, last_states.float_max);

  if (state.getFirst() != nullptr) {
    printAndPublish(MQTT_TOPIC_OUT_ACTIONS, state);
  }
}

#pragma clang diagnostic pop
