#include <tools.h>


Adafruit_PWMServoDriver pwm_driver;

Pump pump_flora_micro(&pwm_driver, 0, KAddressEEPROMDefault + 8 * 0);     // Top    | k=3950
Pump pump_flora_gro(&pwm_driver, 1, KAddressEEPROMDefault + 8 * 2);       //        | k=3660
Pump pump_flora_bloom(&pwm_driver, 2, KAddressEEPROMDefault + 8 * 4);     //        | k=3700
Pump pump_ph_up_reservoir(&pwm_driver, 3, KAddressEEPROMDefault + 8 * 6); //        | k=3740
Pump pump_ph_up_basin(&pwm_driver, 4, KAddressEEPROMDefault + 8 * 8);     //        | k=3730
Pump pump_ph_down_basin(&pwm_driver, 5, KAddressEEPROMDefault + 8 * 12);  // Bottom | k=3750

Relay submersible_pump(A5, HIGH); // Left  | Plug BL | Water Pump
Relay plant_lights(A4, HIGH);     //       | Plug TL | Plant Lights
Relay air_mover(A3, HIGH);        //       | Plug BR | Air Mover
Relay bubbler(A2, HIGH);          //       | Plug TR | Bubbler
Relay r4(A1, HIGH);               //       |
Relay r5(4, HIGH);                //       |
Relay r6(0, HIGH);                //       |
Relay r7(1, HIGH);                // Right |

Relay fan0(12, LOW);
Relay fan1(13, LOW);

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

#if defined(BUTTON_TOGGLE_CALIBRATOR)
  if (button_pump != nullptr && !button_pump->isOn()) {
    is_dosing = false;
  }
#endif
}