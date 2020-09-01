#include <sump.h>
#include <float_sensors.h>
#include <tools.h>
#include <button_actions.h>


Task th_check_pump_state(100, checkPumpState);

void checkPumpState(Task *me) {
  bool pump_on = !float_min_state && thing_on;

  if (thing_on) {
    submersible_pump.on();
    plant_lights.on();
    rail_valves.on();
  } else {
    submersible_pump.off();
    plant_lights.off();
    rail_valves.off();
  }
}