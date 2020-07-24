#include <Arduino.h>
#include <SPI.h> // Must include in main file for WiFiNINA
#include <Wire.h>

#include <BME280I2C.h>
#include <SoftTimer.h>

#include <button_actions.h>
#include <water_sensors.h>
#include <network.h>
#include <tools.h>
#include <box.h>
#include <float_sensors.h>
#include <state.h>
#include <secret/wifi.h>
#include <secret/mqtt.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  tds.setKvalueAddress(0xFF);

  Wire.begin();
  while (!bme.begin()) {
    Serial.println("Could not find BME280I2C sensor!");
    delay(1000);
  }

  switch (bme.chipModel()) {
    case BME280::ChipModel_BME280:
      Serial.println("Found BME280 sensor! Success.");
      break;
    case BME280::ChipModel_BMP280:
      Serial.println("Found BMP280 sensor! No Humidity available.");
      break;
    default:
      Serial.println("Found UNKNOWN sensor! Error!");
      while (true);
  }

  pwm_driver.begin();
  pwm_driver.setPWMFreq(1600);

  // Shut off all pins on the driver in case they may be on.
  for (int i = 0; i < 16; ++i) {
    pwm_driver.setPin(i, 0);
  }

  box.add(box_temperature);
  box.add(box_pressure);
  box.add(box_humidity);

  button_pump = &pump_ph_down_basin;

#ifndef DISABLE_NET
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFiClass::firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the WiFiNina firmware");
  }

  mqtt.setCallback(mqttCallback);

  connectMqttClient();
#endif

//  SoftTimer.add(&th_report_memory);
  SoftTimer.add(&th_run_mqtt_loop);
  SoftTimer.add(&th_request_sensor_readings);
  SoftTimer.add(&th_publish_box_sensor_readings);
  SoftTimer.add(&th_check_if_offables_should_off);
  SoftTimer.add(&th_check_state_changes_and_notify);

  button_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), []() { button_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_min_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(FLOAT_MIN_PIN), []() { float_min_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_max_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(FLOAT_MAX_PIN), []() { float_max_debouncer.pciHandleInterrupt(-1); }, CHANGE);
}
