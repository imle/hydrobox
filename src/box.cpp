#include <box.h>
#include <mqtt_publish.h>
#include <constants/mqtt.h>

#include <debug.h>


void createAndSendBoxSensorMessage(Task *me);
Task task_publish_box_sensor_readings(15000, createAndSendBoxSensorMessage);

SenMLPack pack_box("urn:ctrl:box:");
SenMLFloatRecord box_temperature(SENML_NAME_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord box_pressure(SENML_NAME_PRESSURE, SENML_UNIT_PASCAL);
SenMLFloatRecord box_humidity(SENML_NAME_HUMIDITY, SENML_UNIT_RELATIVE_HUMIDITY);

Adafruit_BME280 bme;
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

bool setupAndCheckBoxHardware() {
  FUNC_IN
#ifdef ARDUINO_AVR_UNO_WIFI_REV2
  while (!bme.begin(BME280_ADDRESS)) {
#else
  while (!bme.begin(BME280_ADDRESS_ALTERNATE)) {
#endif
    Serial.println("Could not find BME280I2C sensor!");
    FUNC_OUT
    return false;
  }

  // We can add these here since the pack_box is a static set of data
  pack_box.add(box_temperature);
  pack_box.add(box_pressure);
  pack_box.add(box_humidity);

  FUNC_OUT
  return true;
}

sensors_event_t event;

void createAndSendBoxSensorMessage(Task *me) {
  FUNC_IN
  bme_temp->getEvent(&event);
  FUNC(false, "getEvent ")
  box_temperature.set(event.temperature);
  FUNC(false, "set ")

  bme_pressure->getEvent(&event);
  FUNC(false, "getEvent ")
  box_pressure.set(event.pressure * 100); // hPa * 100 = Pa
  FUNC(false, "set ")

  bme_humidity->getEvent(&event);
  FUNC(false, "getEvent ")
  box_humidity.set(event.relative_humidity);
  FUNC(false, "set ")

  printAndPublish(MQTT_TOPIC_OUT_BOX, pack_box);
  FUNC_OUT
}
