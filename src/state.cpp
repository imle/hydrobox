#include <state.h>

#include <StringStream.h>

#include <PhysicalStates.h>
#include <float_sensors.h>
#include <network.h>
#include <tools.h>
#include <constants/sensor_data.h>


SenMLPack state(SENML_BASE_NAME_STATE);
SenMLBoolRecord sml_pumps_locked_off(F("pumps_locked_off"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_micro(F("pump_flora_micro"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_gro(F("pump_flora_gro"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_bloom(F("pump_flora_bloom"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_up_reservoir(F("pump_ph_up_reservoir"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_up_basin(F("pump_ph_up_basin"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_down_basin(F("pump_ph_down_basin"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_submersible_pump(F("submersible_pump"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_plant_lights(F("plant_lights"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_air_mover(F("air_mover"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_bubbler(F("bubbler"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r4(F("r4"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r5(F("r5"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r6(F("r6"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r7(F("r7"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_fan0(F("fan0"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_fan1(F("fan1"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_float_min(F("float_min"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_float_max(F("float_max"), SENML_UNIT_RATIO);

Task th_check_state_changes_and_notify(100, checkStateChangesAndSendUpdateMessageIfNecessary);

PhysicalStates last_states;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-non-const-parameter"

void checkStateChangesAndSendUpdateMessageIfNecessary(Task *me) {
  state.clear();

  // If any one pump is locked off, all of them are
  if (last_states.pumps_locked_off != (pump_flora_micro.state() == Pump::State::LOCKED_OFF)) {
    state.add(sml_pump_flora_micro);
    last_states.pumps_locked_off = pump_flora_micro.state() == Pump::State::LOCKED_OFF;
    sml_pumps_locked_off.set(last_states.pumps_locked_off);
  }

  if (last_states.pump_flora_micro != pump_flora_micro.isOn()) {
    state.add(sml_pump_flora_micro);
    last_states.pump_flora_micro = pump_flora_micro.isOn();
    sml_pump_flora_micro.set(last_states.pump_flora_micro);
  }
  if (last_states.pump_flora_gro != pump_flora_gro.isOn()) {
    state.add(sml_pump_flora_gro);
    last_states.pump_flora_gro = pump_flora_gro.isOn();
    sml_pump_flora_gro.set(last_states.pump_flora_gro);
  }
  if (last_states.pump_flora_bloom != pump_flora_bloom.isOn()) {
    state.add(sml_pump_flora_bloom);
    last_states.pump_flora_bloom = pump_flora_bloom.isOn();
    sml_pump_flora_bloom.set(last_states.pump_flora_bloom);
  }
  if (last_states.pump_ph_up_reservoir != pump_ph_up_reservoir.isOn()) {
    state.add(sml_pump_ph_up_reservoir);
    last_states.pump_ph_up_reservoir = pump_ph_up_reservoir.isOn();
    sml_pump_ph_up_reservoir.set(last_states.pump_ph_up_reservoir);
  }
  if (last_states.pump_ph_up_basin != pump_ph_up_basin.isOn()) {
    state.add(sml_pump_ph_up_basin);
    last_states.pump_ph_up_basin = pump_ph_up_basin.isOn();
    sml_pump_ph_up_basin.set(last_states.pump_ph_up_basin);
  }
  if (last_states.pump_ph_down_basin != pump_ph_down_basin.isOn()) {
    state.add(sml_pump_ph_down_basin);
    last_states.pump_ph_down_basin = pump_ph_down_basin.isOn();
    sml_pump_ph_down_basin.set(last_states.pump_ph_down_basin);
  }
  if (last_states.submersible_pump != submersible_pump.isOn()) {
    state.add(sml_submersible_pump);
    last_states.submersible_pump = submersible_pump.isOn();
    sml_submersible_pump.set(last_states.submersible_pump);
  }
  if (last_states.plant_lights != plant_lights.isOn()) {
    state.add(sml_plant_lights);
    last_states.plant_lights = plant_lights.isOn();
    sml_plant_lights.set(last_states.plant_lights);
  }
  if (last_states.air_mover != air_mover.isOn()) {
    state.add(sml_air_mover);
    last_states.air_mover = air_mover.isOn();
    sml_air_mover.set(last_states.air_mover);
  }
  if (last_states.bubbler != bubbler.isOn()) {
    state.add(sml_bubbler);
    last_states.bubbler = bubbler.isOn();
    sml_bubbler.set(last_states.bubbler);
  }
  if (last_states.r4 != r4.isOn()) {
    state.add(sml_r4);
    last_states.r4 = r4.isOn();
    sml_r4.set(last_states.r4);
  }
  if (last_states.r5 != r5.isOn()) {
    state.add(sml_r5);
    last_states.r5 = r5.isOn();
    sml_r5.set(last_states.r5);
  }
  if (last_states.r6 != r6.isOn()) {
    state.add(sml_r6);
    last_states.r6 = r6.isOn();
    sml_r6.set(last_states.r6);
  }
  if (last_states.r7 != r7.isOn()) {
    state.add(sml_r7);
    last_states.r7 = r7.isOn();
    sml_r7.set(last_states.r7);
  }
  if (last_states.fan0 != fan0.isOn()) {
    state.add(sml_fan0);
    last_states.fan0 = fan0.isOn();
    sml_fan0.set(last_states.fan0);
  }
  if (last_states.fan1 != fan1.isOn()) {
    state.add(sml_fan1);
    last_states.fan1 = fan1.isOn();
    sml_fan1.set(last_states.fan1);
  }
  if (last_states.float_min != float_min_state) {
    state.add(sml_float_min);
    last_states.float_min = float_min_state;
    sml_float_min.set(last_states.float_min);
  }
  if (last_states.float_max != float_max_state) {
    state.add(sml_float_max);
    last_states.float_max = float_max_state;
    sml_float_max.set(last_states.float_max);
  }

  if (state.getFirst() != nullptr) {
#if !defined(DISABLE_SERIAL_DEBUG) || !defined(DISABLE_NET)
    StringStream sml_string_stream;
    state.toJson(sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
    Serial.print(MQTT_TOPIC_OUT_ACTIONS " ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
    mqtt.publish(MQTT_TOPIC_OUT_ACTIONS, sml_string_stream.str().c_str());
#endif
  }
}

#pragma clang diagnostic pop
