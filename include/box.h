#ifndef HPCC_BOX_H
#define HPCC_BOX_H

#include <Task.h>
#include <Adafruit_BME280.h>
#include <thingsml.h>

extern Adafruit_BME280 bme;

// Check the BME280 sensor in the control pack_box
extern Task task_publish_box_sensor_readings;

bool setupAndCheckBoxHardware();

#endif //HPCC_BOX_H
