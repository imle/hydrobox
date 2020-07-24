#ifndef HPCC_TOOLS_H
#define HPCC_TOOLS_H

#include <Task.h>
#include <Pump.h>
#include <Relay.h>


extern Adafruit_PWMServoDriver pwm_driver;

// k values are calculated @ 13.3V (on regulator)
extern Pump pump_flora_micro;     // Top    | k=3950
extern Pump pump_flora_gro;       //        | k=3660
extern Pump pump_flora_bloom;     //        | k=3700
extern Pump pump_ph_up_reservoir; //        | k=3740
extern Pump pump_ph_up_basin;     //        | k=3730
extern Pump pump_ph_down_basin;   // Bottom | k=3750

extern Relay submersible_pump; // Left  | Plug BL | Water Pump
extern Relay plant_lights;     //       | Plug TL | Plant Lights
extern Relay air_mover;        //       | Plug TR | Air Mover
extern Relay bubbler;          //       | Plug BR | Bubbler
extern Relay r4;               //       |
extern Relay r5;               //       |
extern Relay r6;               //       |
extern Relay r7;               // Right |

extern Relay fan0;
extern Relay fan1;

// Ensures all relays and pumps are off if their async-time-on has been exceeded
void checkIfToolsShouldOff(Task *me);
extern Task th_check_if_offables_should_off;

#endif //HPCC_TOOLS_H
