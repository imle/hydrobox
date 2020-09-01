#include <sump.h>
#include <float_sensors.h>
#include <tools.h>
#include <button_actions.h>
#include <debug.h>


void checkPumpState(Task *me);
Task task_check_pump_state(100, checkPumpState);

void checkPumpState(Task *me) {
  FUNC_IN
//  bool pump_on = !float_min_state && thing_on;

  if (thing_on) {
    if (!submersible_pump.isOn())
      submersible_pump.on();
    if (!plant_lights.isOn())
      plant_lights.on();
    if (!rail_valves.isOn())
      rail_valves.on();
  } else {
    if (submersible_pump.isOn())
      submersible_pump.off();
    if (plant_lights.isOn())
      plant_lights.off();
    if (rail_valves.isOn())
      rail_valves.off();
  }
  FUNC_OUT
}