#ifndef HPCC_WATER_SENSORS_H
#define HPCC_WATER_SENSORS_H

#include <EzoBoard.h>
#include <Task.h>
#include <DelayRun.h>
#include <GravityTDS.h>
#include <thingsml.h>


// Each Atlas sensor is sent the read command
extern Task task_request_sensor_readings;

// Triggered to run after task_request_sensor_readings runs and will remove itself after it finishes
extern Task task_receive_sensor_readings;

// Triggered once task_receive_sensor_readings has received all sensor readings
extern DelayRun task_create_and_send_water_sensor_message;

void setupAndCheckSensorHardware();

#endif //HPCC_WATER_SENSORS_H
