#include <Arduino.h>
#include <WiFiNINA.h>
#include <SPI.h>

#include <ArduinoJson.h>
#include <AccelStepper.h>
#include <Adafruit_BME280.h>
#include <MQTT.h>
#include <StringStream.h>
#include <senml_pack.h>
#include <kpn_senml.h>

#include <Interval.h>
#include <Pump.h>
#include <Relay.h>
#include <NutrientDosser.h>
#include <FeedChart.h>
#include <EEPROM.h>
#include <PhysicalStates.h>
#include <DosserCalibrator.h>
#include "secret/wifi.h"
#include "secret/mqtt.h"
#include "constants/mqtt.h"
#include "constants/sensor_data.h"


#define DISABLE_NET
#define DISABLE_SERIAL_DEBUG
//#define BUTTON_HOLD_ON
//#define BUTTON_TOGGLE_ON
//#define BUTTON_TOGGLE_CALIBRATOR

char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
uint8_t status = WL_IDLE_STATUS;

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

Adafruit_PWMServoDriver driver;

// k values are calculated @ 13.3V (on regulator)
Pump pump_flora_micro(&driver, 0, KAddressEEPROMDefault + 8 * 0);     // Top    | k=3950
Pump pump_flora_gro(&driver, 1, KAddressEEPROMDefault + 8 * 2);       //        | k=3660
Pump pump_flora_bloom(&driver, 2, KAddressEEPROMDefault + 8 * 4);     //        | k=3700
Pump pump_ph_up_reservoir(&driver, 3, KAddressEEPROMDefault + 8 * 6); //        | k=3740
Pump pump_ph_up_basin(&driver, 4, KAddressEEPROMDefault + 8 * 8);     //        | k=3730
Pump pump_ph_down_basin(&driver, 5, KAddressEEPROMDefault + 8 * 12);  // Bottom | k=3750

Relay submersible_pump(1, HIGH); // Left  | Plug BL | Water Pump
Relay plant_lights(0, HIGH);     //       | Plug TL | Plant Lights
Relay air_mover(A0, HIGH);       //       | Plug TR | Air Mover
Relay r3(A1, HIGH);              //       | Plug BR |
Relay r4(A2, HIGH);              //       |
Relay r5(A3, HIGH);              //       |
Relay r6(A4, HIGH);              //       |
Relay r7(A5, HIGH);              // Right |

Relay fan0(12, LOW);
Relay fan1(13, LOW);

SenMLPack box(SENML_BASE_NAME_BOX);
SenMLFloatRecord temperature(KPN_SENML_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord pressure(KPN_SENML_PRESSURE, SENML_UNIT_PASCAL);
SenMLFloatRecord humidity(KPN_SENML_HUMIDITY, SENML_UNIT_RELATIVE_HUMIDITY);

SenMLPack state(SENML_BASE_NAME_STATE);
SenMLBoolRecord sml_pumps_locked_off(F("pumps_locked_off"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_micro(F("pump_flora_micro"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_gro(F("pump_flora_gro"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_flora_bloom(F("pump_flora_bloom"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_up_reservoir(F("pump_ph_up_reservoir"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_up_basin(F("pump_ph_up_basin"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_pump_ph_down_basin(F("pump_ph_down_basin"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_submersible_pump(F("submersible_pump"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_plant_lights(F("plant_lights"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_air_mover(F("air_mover"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r3(F("r3"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r4(F("r4"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r5(F("r5"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r6(F("r6"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_r7(F("r7"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_fan0(F("fan0"), SENML_UNIT_RATIO);
SenMLBoolRecord sml_fan1(F("fan1"), SENML_UNIT_RATIO);

void callback(MQTTClient *client, char topic[], char bytes[], int length);

void checkWifiModule();

void connectMqttClient();

void setupStaticSenMLObjects();

void checkStateChangesAndSendUpdateMessageIfNecessary();

void createAndSendSensorMessage();

String mqtt_server = "mqtt.imle.io";
MQTTClient mqtt;

Interval change_interval{
    .PUB_INTERVAL = 5000,
};
Interval box_interval{
    .PUB_INTERVAL = 5000,
};

Relay *mixers[] = {&fan0, &fan1};

DosserCalibrator calibrator;

NutrientDosser dosser(DefaultFeedChart(),
                      pump_flora_micro, pump_flora_gro, pump_flora_bloom,
                      pump_ph_up_reservoir, UP,
                      mixers, 2);

StaticJsonDocument<200> doc;

#if defined(BUTTON_TOGGLE_ON) || defined(BUTTON_HOLD_ON) || defined(BUTTON_TOGGLE_CALIBRATOR)
Pump *button_pump;
#endif
#ifdef BUTTON_TOGGLE_CALIBRATOR
bool should_dose = false;
bool is_dosing = false;
#endif

#ifdef BUTTON_TOGGLE_ON
void toggleButton() {
  if (button_pump->isOn()) {
    button_pump->off();
  } else {
    button_pump->on();
  }
}
#elif defined(BUTTON_TOGGLE_CALIBRATOR)
void toggleButton() {
  if (!is_dosing) {
    should_dose = true;
  }
}
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (true);
  }

  driver.begin();
  driver.setPWMFreq(1600);

  // Shut off all pins on the driver in case they may be on.
  for (int i = 0; i < 16; ++i) {
    driver.setPin(i, 0);
  }

  setupStaticSenMLObjects();

#if defined(BUTTON_HOLD_ON) || defined(BUTTON_TOGGLE_ON) || defined(BUTTON_TOGGLE_CALIBRATOR)
  button_pump = &pump_ph_down_basin;
#endif

#ifndef DISABLE_NET
  checkWifiModule();

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to wifi");

  mqtt.begin(mqtt_server.c_str(), wifiClient);
  mqtt.onMessageAdvanced(callback);

  connectMqttClient();

  mqtt.subscribe(MQTT_TOPIC_IN_COMMAND);
  mqtt.subscribe(MQTT_TOPIC_IN_STATUS);
#endif

#if defined(BUTTON_HOLD_ON) || defined(BUTTON_TOGGLE_ON) || defined(BUTTON_TOGGLE_CALIBRATOR)
  pinMode(2, INPUT_PULLDOWN);
#endif

#ifdef BUTTON_HOLD_ON
  pinMode(2, INPUT_PULLDOWN);
#elif defined(BUTTON_TOGGLE_ON) || defined(BUTTON_TOGGLE_CALIBRATOR)
  attachInterrupt(digitalPinToInterrupt(2), toggleButton, RISING);
#endif
}

void callback(MQTTClient *client, char topic[], char bytes[], int length) {
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(topic);
  Serial.print(" ");
  for (int i = 0; i < length; ++i) {
    Serial.print(bytes[i]);
  }
  Serial.println();
#endif

  DeserializationError error = deserializeJson(doc, bytes, length);
  if (error != DeserializationError::Ok) {
    Serial.println(error.c_str());
    return;
  }

  if (topic == MQTT_TOPIC_IN_STATUS) {

  }
}

void loop() {
#if defined(BUTTON_TOGGLE_ON) || defined(BUTTON_HOLD_ON) || defined(BUTTON_TOGGLE_CALIBRATOR)
  if (Serial.available()) {
    long val = Serial.parseInt();
    Serial.println(val);
    button_pump->setCalibrationKValue(val);
#ifdef BUTTON_TOGGLE_ON
    if (button_pump->isOn()) {
      // Turn on again to set new k value
      button_pump->on();
    }
#endif
  }
#endif

  if (box_interval.last_pub_ms + box_interval.PUB_INTERVAL < millis()) {
    box_interval.last_pub_ms = millis();
    createAndSendSensorMessage();
  }

#ifndef DISABLE_NET
  mqtt.loop();

  if (!mqtt.connected()) {
    connectMqttClient();
  }
#endif

#ifdef BUTTON_TOGGLE_CALIBRATOR
  if (should_dose && !is_dosing) {
    is_dosing = true;
    should_dose = false;
    calibrator.dose(*button_pump);
    is_dosing = false;
  }
#endif

#ifdef BUTTON_HOLD_ON
  if (button_pump != nullptr) {
    if (digitalRead(2) == HIGH) {
      if (!button_pump->isOn()) {
        button_pump->on();
      }
    } else {
      if (button_pump->isOn()) {
        button_pump->off();
      }
    }
  }
#endif

  checkStateChangesAndSendUpdateMessageIfNecessary();

#ifndef DISABLE_NET
  if (change_interval.last_pub_ms + change_interval.PUB_INTERVAL < millis()) {
    change_interval.last_pub_ms = millis();
    mqtt.publish(MQTT_TOPIC_OUT_SENSORS, "{\"sensor\":2}");
  }
#endif

  delay(20);
}

void checkWifiModule() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFiClass::firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
}

void connectMqttClient() {
  Serial.print("Attempting to connect to MQTT server at: ");
  Serial.println(mqtt_server);
  if (!mqtt.connect(SECRET_MQTT_CLIENT_ID)) {
//  if (!mqtt.connect(SECRET_MQTT_CLIENT_ID, SECRET_MQTT_USER, SECRET_MQTT_PASS)) {
    Serial.println("Unable to authenticate against mqtt endpoint.");
    Serial.println(mqtt.lastError());
    while (true);
  }
  Serial.println("Connected to mqtt server.");
}

PhysicalStates last_states;

void checkStateChangesAndSendUpdateMessageIfNecessary() {
  state.clear();

  // If any one pump is locked off, all of them are
  if (last_states.pumps_locked_off != (pump_flora_micro.state() == Pump::State::LOCKED_OFF)) {
    state.add(&sml_pump_flora_micro);
    last_states.pumps_locked_off = pump_flora_micro.state() == Pump::State::LOCKED_OFF;
    sml_pumps_locked_off.set(last_states.pumps_locked_off);
  }

  if (last_states.pump_flora_micro != pump_flora_micro.isOn()) {
    state.add(&sml_pump_flora_micro);
    last_states.pump_flora_micro = pump_flora_micro.isOn();
    sml_pump_flora_micro.set(last_states.pump_flora_micro);
  }
  if (last_states.pump_flora_gro != pump_flora_gro.isOn()) {
    state.add(&sml_pump_flora_gro);
    last_states.pump_flora_gro = pump_flora_gro.isOn();
    sml_pump_flora_gro.set(last_states.pump_flora_gro);
  }
  if (last_states.pump_flora_bloom != pump_flora_bloom.isOn()) {
    state.add(&sml_pump_flora_bloom);
    last_states.pump_flora_bloom = pump_flora_bloom.isOn();
    sml_pump_flora_bloom.set(last_states.pump_flora_bloom);
  }
  if (last_states.pump_ph_up_reservoir != pump_ph_up_reservoir.isOn()) {
    state.add(&sml_pump_ph_up_reservoir);
    last_states.pump_ph_up_reservoir = pump_ph_up_reservoir.isOn();
    sml_pump_ph_up_reservoir.set(last_states.pump_ph_up_reservoir);
  }
  if (last_states.pump_ph_up_basin != pump_ph_up_basin.isOn()) {
    state.add(&sml_pump_ph_up_basin);
    last_states.pump_ph_up_basin = pump_ph_up_basin.isOn();
    sml_pump_ph_up_basin.set(last_states.pump_ph_up_basin);
  }
  if (last_states.pump_ph_down_basin != pump_ph_down_basin.isOn()) {
    state.add(&sml_pump_ph_down_basin);
    last_states.pump_ph_down_basin = pump_ph_down_basin.isOn();
    sml_pump_ph_down_basin.set(last_states.pump_ph_down_basin);
  }
  if (last_states.submersible_pump != submersible_pump.isOn()) {
    state.add(&sml_submersible_pump);
    last_states.submersible_pump = submersible_pump.isOn();
    sml_submersible_pump.set(last_states.submersible_pump);
  }
  if (last_states.plant_lights != plant_lights.isOn()) {
    state.add(&sml_plant_lights);
    last_states.plant_lights = plant_lights.isOn();
    sml_plant_lights.set(last_states.plant_lights);
  }
  if (last_states.air_mover != air_mover.isOn()) {
    state.add(&sml_air_mover);
    last_states.air_mover = air_mover.isOn();
    sml_air_mover.set(last_states.air_mover);
  }
  if (last_states.r3 != r3.isOn()) {
    state.add(&sml_r3);
    last_states.r3 = r3.isOn();
    sml_r3.set(last_states.r3);
  }
  if (last_states.r4 != r4.isOn()) {
    state.add(&sml_r4);
    last_states.r4 = r4.isOn();
    sml_r4.set(last_states.r4);
  }
  if (last_states.r5 != r5.isOn()) {
    state.add(&sml_r5);
    last_states.r5 = r5.isOn();
    sml_r5.set(last_states.r5);
  }
  if (last_states.r6 != r6.isOn()) {
    state.add(&sml_r6);
    last_states.r6 = r6.isOn();
    sml_r6.set(last_states.r6);
  }
  if (last_states.r7 != r7.isOn()) {
    state.add(&sml_r7);
    last_states.r7 = r7.isOn();
    sml_r7.set(last_states.r7);
  }
  if (last_states.fan0 != fan0.isOn()) {
    state.add(&sml_fan0);
    last_states.fan0 = fan0.isOn();
    sml_fan0.set(last_states.fan0);
  }
  if (last_states.fan1 != fan1.isOn()) {
    state.add(&sml_fan1);
    last_states.fan1 = fan1.isOn();
    sml_fan1.set(last_states.fan1);
  }

  if (state.getFirst() != nullptr) {
#if !defined(DISABLE_SERIAL_DEBUG) && !defined(DISABLE_NET)
    StringStream sml_string_stream;
    sml_string_stream.flush();
    state.toJson(&sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
    Serial.print(MQTT_TOPIC_OUT_ACTIONS " ");
    Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
    Serial.println(mqtt.publish(MQTT_TOPIC_OUT_ACTIONS, sml_string_stream.str()));
#endif
  }
}

sensors_event_t sensor_event;

// https://tools.ietf.org/html/rfc8428#section-12.1
void createAndSendSensorMessage() {
  bme_temp->getEvent(&sensor_event);
  temperature.set(sensor_event.temperature);

  bme_pressure->getEvent(&sensor_event);
  pressure.set(sensor_event.pressure * 100); // read unit is hPa (*100 to get Pa)

  bme_humidity->getEvent(&sensor_event);
  humidity.set(sensor_event.relative_humidity);

#if !defined(DISABLE_SERIAL_DEBUG) && !defined(DISABLE_NET)
  StringStream sml_string_stream;
  sml_string_stream.flush();
  box.toJson(&sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(MQTT_TOPIC_OUT_SENSORS " ");
  Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
  Serial.println(mqtt.publish(MQTT_TOPIC_OUT_SENSORS, sml_string_stream.str()));
#endif
}

void setupStaticSenMLObjects() {
  box.add(&temperature);
  box.add(&pressure);
  box.add(&humidity);
}
