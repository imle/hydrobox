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

Relay mixers(PIN_FAN0, LOW);

Relay rail_valves(PIN_RAIL_VALVES, LOW);