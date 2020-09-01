#include <tools.h>
#include <pin_assignment.h>


PCA9685 pwm_driver;

Pump pump_flora_micro(&pwm_driver, 0, 3950);     // Top    | k=3950
Pump pump_flora_gro(&pwm_driver, 1, 3660);       //        | k=3660
Pump pump_flora_bloom(&pwm_driver, 2, 3700);     //        | k=3700
Pump pump_ph_up_reservoir(&pwm_driver, 3, 3740); //        | k=3740
Pump pump_ph_up_basin(&pwm_driver, 4, 3730);     //        | k=3730
Pump pump_ph_down_basin(&pwm_driver, 5, 3750);   // Bottom | k=3750

Relay submersible_pump(PIN_SUBMERSIBLE_PUMP, HIGH); // Left  | Plug BL | Water Pump
Relay plant_lights(PIN_PLANT_LIGHTS, HIGH);         //       | Plug TL | Plant Lights
Relay air_mover(PIN_AIR_MOVER, HIGH);               //       | Plug BR | Air Mover
Relay bubbler(PIN_BUBBLER, HIGH);                   //       | Plug TR | Bubbler
Relay r4(PIN_R4, HIGH);                             //       |
Relay r5(PIN_R5, HIGH);                             //       |
Relay r6(PIN_R6, HIGH);                             //       |
Relay r7(PIN_R7, HIGH);                             // Right |

Relay fan0(PIN_FAN0, LOW);
Relay fan1(PIN_FAN1, LOW);

Relay rail_valves(PIN_RAIL_VALVES, LOW);

Task th_check_if_offables_should_off(50, checkIfToolsShouldOff);

void checkIfToolsShouldOff(Task *me) {
  pump_flora_micro.checkShouldOff();
  pump_flora_gro.checkShouldOff();
  pump_flora_bloom.checkShouldOff();
  pump_ph_up_reservoir.checkShouldOff();
  pump_ph_up_basin.checkShouldOff();
  pump_ph_down_basin.checkShouldOff();

  submersible_pump.checkShouldOff();
  plant_lights.checkShouldOff();
  air_mover.checkShouldOff();
  bubbler.checkShouldOff();
  r4.checkShouldOff();
  r5.checkShouldOff();
  r6.checkShouldOff();
  r7.checkShouldOff();

  fan0.checkShouldOff();
  fan1.checkShouldOff();

  rail_valves.checkShouldOff();

#if defined(BUTTON_TOGGLE_CALIBRATOR)
  if (button_pump != nullptr && !button_pump->isOn()) {
    is_dosing = false;
  }
#endif
}