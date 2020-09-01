#include <Arduino.h>
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
#include <sump.h>
#include <pin_assignment.h>


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Startup...");

#ifndef DISABLE_NET
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFiClass::firmwareVersion();
  Serial.print("WiFiNINA Firmware Version: ");
  Serial.println(fv);
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the WiFiNina firmware");
  }

  uint8_t status = WL_IDLE_STATUS;

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.print("You're connected to the network | time: ");
  Serial.println(WiFi.getTime());
  printCurrentNet();
  printWifiData();

  mqtt.setServer(MQTT_SERVER, 1883);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(350);
#endif

  tds.setKvalueAddress(0xFF);

  Wire.begin();
  while (!bme.begin()) {
    Serial.println("Could not find BME280I2C sensor!");
    delay(1000);
  }

  switch (bme.chipModel()) {
    case BME280::ChipModel_BME280:
      has_humidity = true;
      break;
    case BME280::ChipModel_BMP280:
      has_humidity = false;
      break;
    default:
      Serial.print("I2C device at 0x");
      Serial.print(bme.getSettings().bme280Addr, HEX);
      Serial.println(" is not BME280!");
      while (true);
  }

  pwm_driver.setup();
  pwm_driver.setFrequency(1600);

  // Shut off all pins on the driver in case they may be on.
  for (int i = 0; i < 16; ++i) {
    pwm_driver.getPin(i).fullOff();
  }
  pwm_driver.writeAllPins();

  box.add(box_temperature);
  box.add(box_pressure);
  if (has_humidity) {
    box.add(box_humidity);
  }



#ifndef DISABLE_NET
  SoftTimer.add(&th_run_mqtt_loop);
  SoftTimer.add(&th_run_mqtt_reconnect_check);
#endif

  SoftTimer.add(&th_check_pump_state);
  SoftTimer.add(&th_request_sensor_readings);
  SoftTimer.add(&th_publish_box_sensor_readings);
  SoftTimer.add(&th_check_if_offables_should_off);
  SoftTimer.add(&th_check_state_changes_and_notify);

  button_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), []() { button_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_rails_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT_RAILS), []() { float_rails_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_min_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT_MIN), []() { float_min_debouncer.pciHandleInterrupt(-1); }, CHANGE);

  float_max_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT_MAX), []() { float_max_debouncer.pciHandleInterrupt(-1); }, CHANGE);
}
