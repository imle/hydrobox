#include <box.h>

#include <BME280I2C.h>
#include <StringStream.h>

#include <network.h>
#include <constants/sensor_data.h>
#include <constants/mqtt.h>


Task th_publish_box_sensor_readings(15000, createAndSendBoxSensorMessage);

SenMLPack box(SENML_BASE_NAME_BOX);
SenMLFloatRecord box_temperature(SENML_NAME_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord box_pressure(SENML_NAME_PRESSURE, SENML_UNIT_PASCAL);
SenMLFloatRecord box_humidity(SENML_NAME_HUMIDITY, SENML_UNIT_RELATIVE_HUMIDITY);

float temp, press, humid;

BME280I2C::Settings bme_settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    BME280I2C::I2CAddr_0x77 // I2C address. I2C specific.
);

BME280I2C bme(bme_settings);

void createAndSendBoxSensorMessage(Task *me) {
  bme.read(temp, press, humid, BME280::TempUnit_Celsius, BME280::PresUnit_Pa);

  box_temperature.set(temp);
  box_pressure.set(press);
  box_humidity.set(humid);

#if !defined(DISABLE_SERIAL_DEBUG) || !defined(DISABLE_NET)
  StringStream sml_string_stream;
  box.toJson(sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(MQTT_TOPIC_OUT_SENSORS " ");
  Serial.print(millis());
  Serial.print(" ");
  Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
  mqtt.publish(MQTT_TOPIC_OUT_SENSORS, sml_string_stream.str().c_str());
#endif
}
