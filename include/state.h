#ifndef HPCC_STATE_H
#define HPCC_STATE_H

#include <Task.h>
#include <thingsml.h>


// https://tools.ietf.org/html/rfc8428#section-12.1
extern SenMLPack state;
extern SenMLBoolRecord sml_pump_flora_micro;
extern SenMLBoolRecord sml_pump_flora_gro;
extern SenMLBoolRecord sml_pump_flora_bloom;
extern SenMLBoolRecord sml_pump_ph_up_reservoir;
extern SenMLBoolRecord sml_pump_ph_up_basin;
extern SenMLBoolRecord sml_pump_ph_down_basin;
extern SenMLBoolRecord sml_submersible_pump;
extern SenMLBoolRecord sml_plant_lights;
extern SenMLBoolRecord sml_air_mover;
extern SenMLBoolRecord sml_bubbler;
extern SenMLBoolRecord sml_r4;
extern SenMLBoolRecord sml_r5;
extern SenMLBoolRecord sml_r6;
extern SenMLBoolRecord sml_r7;
extern SenMLBoolRecord sml_fan0;
extern SenMLBoolRecord sml_fan1;
extern SenMLBoolRecord sml_rail_valves;
extern SenMLBoolRecord sml_float_rails;
extern SenMLBoolRecord sml_float_min;
extern SenMLBoolRecord sml_float_max;

// Check for changed state of each relay and pump and publish a state change message if necessary
extern Task task_check_state_changes_and_notify;

#endif //HPCC_STATE_H
