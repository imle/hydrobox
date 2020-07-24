#ifndef HPCC_WATER_SENSORS_H
#define HPCC_WATER_SENSORS_H

#include <EzoBoard.h>
#include <Task.h>
#include <DelayRun.h>
#include <GravityTDS.h>
#include <thingsml.h>


extern GravityTDS tds;

extern EzoBoard ezo_ph_basin;     // Probes in the lower water basin
extern EzoBoard ezo_ec_basin;     // Probes in the lower water basin
extern EzoBoard ezo_temp_basin;   // Probes in the lower water basin
extern EzoBoard ezo_ph_reservoir; // Probe in the water mixing tank

#define SENML_BASE_NAME_SENSORS "urn:ctrl:sensor:"

// https://tools.ietf.org/html/rfc8428#section-12.1
extern SenMLPack sensors;
extern SenMLFloatRecord sensors_ph_basin;
extern SenMLFloatRecord sensors_ec_basin;
extern SenMLFloatRecord sensors_temp_basin;
extern SenMLFloatRecord sensors_ph_reservoir;
extern SenMLFloatRecord sensors_ec_reservoir;
extern SenMLFloatRecord sensors_temp_reservoir;

// Each Atlas sensor is sent the read command
void requestWaterSensorReadings(Task *me);
extern Task th_request_sensor_readings;

// Triggered to run after th_request_sensor_readings runs and will remove itself after it finishes
void receiveAtlasSensorReadings(Task *me);
extern Task th_receive_sensor_readings;

// Triggered once th_receive_sensor_readings has received all sensor readings
bool createAndSendWaterSensorsMessage(Task *me);
extern DelayRun th_create_and_send_water_sensor_message;

#endif //HPCC_WATER_SENSORS_H
