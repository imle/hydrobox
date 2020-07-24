#ifndef HPCC_BOX_H
#define HPCC_BOX_H

#include <Task.h>
#include <BME280I2C.h>
#include <senml_pack.h>
#include <kpn_senml.h>


// https://tools.ietf.org/html/rfc8428#section-12.1
extern SenMLPack box;
extern SenMLFloatRecord box_temperature;
extern SenMLFloatRecord box_pressure;
extern SenMLFloatRecord box_humidity;

extern BME280I2C bme;

// Check the BME280 sensor in the control box
void createAndSendBoxSensorMessage(Task *me);
extern Task th_publish_box_sensor_readings;

#endif //HPCC_BOX_H
