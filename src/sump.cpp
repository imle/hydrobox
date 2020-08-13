#include <sump.h>
#include <float_sensors.h>
#include <tools.h>
#include <button_actions.h>


Task th_check_pump_state(100, checkPumpState);

void checkPumpState(Task *me) {
  bool pump_on = true;

  if (float_min_state) {
    pump_on = false;
  } else if (!thing_on) {
    pump_on = false;
  }

  if (pump_on) {
    submersible_pump.on();
    plant_lights.on();
  } else {
    submersible_pump.off();
    plant_lights.off();
  }
}