#include <Arduino.h>
#include <SPI.h> // Must include for WiFiNINA
#include <WiFiNINA.h>

#include <Wire.h>
#include <MQTT.h>
#include <StringStream.h>
#include <ArduinoJson.h>
#include <senml_pack.h>
#include <kpn_senml.h>
#include <Adafruit_BME280.h>
#include <SoftTimer.h>
#include <DelayRun.h>
#include <Debouncer.h>
#include <Task.h>

#include <Pump.h>
#include <Relay.h>
#include <NutrientDosser.h>
#include <FeedChart.h>
#include <EEPROM.h>
#include <PhysicalStates.h>
#include <button_actions.h>
#include <Ezo.h>
#include "secret/wifi.h"
#include "secret/mqtt.h"
#include "constants/mqtt.h"
#include "constants/sensor_data.h"


#define DISABLE_NET
//#define DISABLE_SERIAL_DEBUG

#define BUTTON_PIN 2
void pinChanged();
Debouncer button_debouncer(BUTTON_PIN, MODE_OPEN_ON_PUSH, buttonPressed, buttonReleased);

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

Adafruit_PWMServoDriver driver;

EzoBoard ezo_ph_basin = EzoBoard(10, "PH Basin"); // Probes in the lower water basin
EzoBoard ezo_ec_basin = EzoBoard(12, "EC Basin"); // Probes in the lower water basin
EzoBoard ezo_temp_basin = EzoBoard(13, "Temp Basin"); // Probes in the lower water basin
EzoBoard ezo_ph_reservoir = EzoBoard(11, "PH Reservoir"); // Probe in the water mixing tank

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

// https://tools.ietf.org/html/rfc8428#section-12.1
SenMLPack box(SENML_BASE_NAME_BOX);
SenMLFloatRecord box_temperature(KPN_SENML_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord box_pressure(KPN_SENML_PRESSURE, SENML_UNIT_PASCAL);
SenMLFloatRecord box_humidity(KPN_SENML_HUMIDITY, SENML_UNIT_RELATIVE_HUMIDITY);

// https://tools.ietf.org/html/rfc8428#section-12.1
// Strange issue with library overwriting the unit
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

// https://tools.ietf.org/html/rfc8428#section-12.1
SenMLPack sensors(SENML_BASE_NAME_SENSORS);
SenMLFloatRecord sensors_ph_basin(F("basin:ph"), SENML_UNIT_PH);
SenMLFloatRecord sensors_ec_basin(F("basin:ec"), SENML_UNIT_SIEMENS);
SenMLFloatRecord sensors_temp_basin(F("basin:" KPN_SENML_TEMPERATURE), SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord sensors_ph_reservoir(F("reservoir:ph"), SENML_UNIT_PH);

void connectMqttClient();

// Runs often to check for incoming mqtt messages and send those ready for transmission
void runMqttLoop(Task *me);
Task th_run_mqtt_loop(100, runMqttLoop);

// Ensures all relays and pumps are off if their async-time-on has been exceeded
void checkIfOffablesShouldOff(Task *me);
Task th_check_if_offables_should_off(50, checkIfOffablesShouldOff);

// Check the BME280 sensor in the control box
void createAndSendBoxSensorMessage(Task *me);
Task th_publish_box_sensor_readings(15000, createAndSendBoxSensorMessage);

// Check for changed state of each relay and pump and publish a state change message if necessary
void checkStateChangesAndSendUpdateMessageIfNecessary(Task *me);
Task th_check_state_changes_and_notify(100, checkStateChangesAndSendUpdateMessageIfNecessary);

// Each Atlas sensor is sent the read command
void requestWaterSensorReadings(Task *me);
Task th_request_sensor_readings(30000, requestWaterSensorReadings);

// Triggered to run after th_request_sensor_readings runs and will remove itself after it finishes
void receiveAtlasSensorReadings(Task *me);
Task th_receive_sensor_readings(100, receiveAtlasSensorReadings);

// Triggered once th_receive_sensor_readings has received all sensor readings
bool createAndSendWaterSensorsMessage(Task *me);
DelayRun th_create_and_send_water_sensor_message(0, createAndSendWaterSensorsMessage);


/// START: Nutrient Dossing Actions
/// Defined in reverse order to take advantage of the DelayRun `followedBy` field

// 6: Last the reservoir will be flushed to the basin
bool flushReservoirToBasin(Task *me);
DelayRun th_flush_reservoir_to_basin(6000, flushReservoirToBasin);

// 5: After about 30 minutes, the pH should be balanced
bool stopPhBalancing(Task *me);
DelayRun th_stop_ph_balancing(1800000, stopPhBalancing, &th_flush_reservoir_to_basin);

// 4.1: Added when th_start_ph_balancing has been run and will be removed after th_stop_ph_balancing runs
void dossingBalanceBasinPh(Task *me);
Task th_balance_ph(1000, dossingBalanceBasinPh);

// 4: Start the pH balancing process after 2 minutes of letting the nutrients disperse
// not actually called by th_start_dossing, triggered by pumps finishing
bool startPhBalancing(Task *me);
DelayRun th_start_ph_balancing(120000, startPhBalancing, &th_stop_ph_balancing);

// 3.1: Added when th_start_dossing has been run and will be removed when all dossing pumps are finished
void dossingCheckState(Task *me);
Task th_check_dossing_state(10, dossingCheckState);

// 3: Begin dossing the reservoir with the General Hydroponics nutrients
bool startDossing(Task *me);
DelayRun th_start_dossing(10000, startDossing);

// 2: Stop the mixing fans
bool stopMixNutrients(Task *me);
DelayRun th_stop_mix_nutrients(15000, stopMixNutrients, &th_start_dossing);

// 1: Start the mixing fans
bool startMixNutrients(Task *me);
DelayRun th_start_mix_nutrients(0, startMixNutrients, &th_stop_mix_nutrients);

/// END: Nutrient Dossing Actions

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifi;
uint8_t status = WL_IDLE_STATUS;

String mqtt_server = "mqtt.imle.io";
MQTTClient mqtt;
void mqttCallback(MQTTClient *client, char topic[], char bytes[], int length);

Relay *mixers[] = {&fan0, &fan1};

NutrientDosser dosser(DefaultFeedChart(),
                      pump_flora_micro, pump_flora_gro, pump_flora_bloom,
                      pump_ph_up_reservoir, UP,
                      mixers, 2);

StaticJsonDocument<200> doc;

Pump *button_pump;
bool is_dosing = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (true);
  }

  Wire.begin();

  driver.begin();
  driver.setPWMFreq(1600);

  // Shut off all pins on the driver in case they may be on.
  for (int i = 0; i < 16; ++i) {
    driver.setPin(i, 0);
  }

  box.add(&box_temperature);
  box.add(&box_pressure);
  box.add(&box_humidity);

  button_pump = &pump_ph_down_basin;

#ifndef DISABLE_NET
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFiClass::firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to wifi");

  mqtt.begin(mqtt_server.c_str(), wifi);
  mqtt.onMessageAdvanced(mqttCallback);

  connectMqttClient();

  mqtt.subscribe(MQTT_TOPIC_IN_COMMAND);
  mqtt.subscribe(MQTT_TOPIC_IN_STATUS);
#endif

  SoftTimer.add(&th_run_mqtt_loop);
  SoftTimer.add(&th_request_sensor_readings);
  SoftTimer.add(&th_publish_box_sensor_readings);
  SoftTimer.add(&th_check_if_offables_should_off);
  SoftTimer.add(&th_check_state_changes_and_notify);

  button_debouncer.init();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pinChanged, CHANGE);
}

void pinChanged() {
  button_debouncer.pciHandleInterrupt(-1);
}

void mqttCallback(MQTTClient *client, char topic[], char bytes[], int length) {
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

float basin_last_temperature = NAN;

void requestWaterSensorReadings(Task *me) {
  // If we don't have compensation yet, don't worry about it
  if (basin_last_temperature != NAN) {
    ezo_ec_basin.sendReadWithTempComp(basin_last_temperature);
  } else {
    // TODO: Ensure this will be ok
    ezo_ec_basin.sendReadCmd();
  }

  ezo_ph_basin.sendReadCmd();
  ezo_temp_basin.sendReadCmd();
  ezo_ph_reservoir.sendReadCmd(); // TODO: Only care about this sensor when dossing

  SoftTimer.add(&th_receive_sensor_readings);
}

float printFailureOnSensor(EzoBoard &sensor) {
  Serial.print(F("Failed reading sensor "));
  Serial.println(sensor.getName());
}

void receiveAtlasSensorReadings(Task *me) {
  EzoBoard::Error status;
  bool finished = true;

  if (ezo_ph_basin.isRequestPending()) {
    status = ezo_ph_basin.receiveReadCmd();
    finished = status != EzoBoard::NOT_READY;
    if (status == EzoBoard::FAIL) {
      printFailureOnSensor(ezo_ph_basin);
    }
  }
  if (ezo_temp_basin.isRequestPending()) {
    status = ezo_temp_basin.receiveReadCmd();
    finished = status != EzoBoard::NOT_READY;
    if (status == EzoBoard::FAIL) {
      printFailureOnSensor(ezo_temp_basin);
    }
  }
  if (ezo_ec_basin.isRequestPending()) {
    status = ezo_ec_basin.receiveReadCmd();
    finished = status != EzoBoard::NOT_READY;
    if (status == EzoBoard::FAIL) {
      printFailureOnSensor(ezo_ec_basin);
    }
  }
  if (ezo_ph_reservoir.isRequestPending()) {
    status = ezo_ph_reservoir.receiveReadCmd();
    finished = status != EzoBoard::NOT_READY;
    if (status == EzoBoard::FAIL) {
      printFailureOnSensor(ezo_ph_reservoir);
    }
  }

  if (finished) {
    SoftTimer.remove(&th_receive_sensor_readings);
    th_create_and_send_water_sensor_message.startDelayed();
  }
}

bool createAndSendWaterSensorsMessage(Task *me) {
  sensors.clear();

  if (ezo_ph_basin.getError() == EzoBoard::SUCCESS) {
    sensors.add(&sensors_ph_basin);
    sensors_ph_basin.setUnit(SENML_UNIT_PH);
    sensors_ph_basin.set(ezo_ph_basin.getLastReceivedReading());
  }
  if (ezo_temp_basin.getError() == EzoBoard::SUCCESS) {
    sensors.add(&sensors_temp_basin);
    sensors_temp_basin.setUnit(SENML_UNIT_DEGREES_CELSIUS);
    sensors_temp_basin.set(ezo_temp_basin.getLastReceivedReading());
  }
  if (ezo_ph_reservoir.getError() == EzoBoard::SUCCESS) {
    sensors.add(&sensors_ph_reservoir);
    sensors_ph_reservoir.setUnit(SENML_UNIT_PH);
    sensors_ph_reservoir.set(ezo_ph_reservoir.getLastReceivedReading());
  }
  if (ezo_ec_basin.getError() == EzoBoard::SUCCESS) {
    sensors.add(&sensors_ec_basin);
    sensors_ec_basin.setUnit(SENML_UNIT_SIEMENS);
    sensors_ec_basin.set(ezo_ec_basin.getLastReceivedReading());
  }

  // Necessary for the ec meter
  basin_last_temperature = sensors_temp_basin.get();

#if !defined(DISABLE_SERIAL_DEBUG) || !defined(DISABLE_NET)
  StringStream sml_string_stream;
  sensors.toJson(&sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(MQTT_TOPIC_OUT_SENSORS " ");
  Serial.print(millis());
  Serial.print(" ");
  Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
  Serial.println(mqtt.publish(MQTT_TOPIC_OUT_SENSORS, sml_string_stream.str()));
#endif
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

void checkStateChangesAndSendUpdateMessageIfNecessary(Task *me) {
  state.clear();

  // If any one pump is locked off, all of them are
  if (last_states.pumps_locked_off != (pump_flora_micro.state() == Pump::State::LOCKED_OFF)) {
    sml_pump_flora_micro.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_flora_micro);
    last_states.pumps_locked_off = pump_flora_micro.state() == Pump::State::LOCKED_OFF;
    sml_pumps_locked_off.set(last_states.pumps_locked_off);
  }

  if (last_states.pump_flora_micro != pump_flora_micro.isOn()) {
    sml_pump_flora_micro.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_flora_micro);
    last_states.pump_flora_micro = pump_flora_micro.isOn();
    sml_pump_flora_micro.set(last_states.pump_flora_micro);
  }
  if (last_states.pump_flora_gro != pump_flora_gro.isOn()) {
    sml_pump_flora_gro.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_flora_gro);
    last_states.pump_flora_gro = pump_flora_gro.isOn();
    sml_pump_flora_gro.set(last_states.pump_flora_gro);
  }
  if (last_states.pump_flora_bloom != pump_flora_bloom.isOn()) {
    sml_pump_flora_bloom.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_flora_bloom);
    last_states.pump_flora_bloom = pump_flora_bloom.isOn();
    sml_pump_flora_bloom.set(last_states.pump_flora_bloom);
  }
  if (last_states.pump_ph_up_reservoir != pump_ph_up_reservoir.isOn()) {
    sml_pump_ph_up_reservoir.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_ph_up_reservoir);
    last_states.pump_ph_up_reservoir = pump_ph_up_reservoir.isOn();
    sml_pump_ph_up_reservoir.set(last_states.pump_ph_up_reservoir);
  }
  if (last_states.pump_ph_up_basin != pump_ph_up_basin.isOn()) {
    sml_pump_ph_up_basin.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_ph_up_basin);
    last_states.pump_ph_up_basin = pump_ph_up_basin.isOn();
    sml_pump_ph_up_basin.set(last_states.pump_ph_up_basin);
  }
  if (last_states.pump_ph_down_basin != pump_ph_down_basin.isOn()) {
    sml_pump_ph_down_basin.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_pump_ph_down_basin);
    last_states.pump_ph_down_basin = pump_ph_down_basin.isOn();
    sml_pump_ph_down_basin.set(last_states.pump_ph_down_basin);
  }
  if (last_states.submersible_pump != submersible_pump.isOn()) {
    sml_submersible_pump.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_submersible_pump);
    last_states.submersible_pump = submersible_pump.isOn();
    sml_submersible_pump.set(last_states.submersible_pump);
  }
  if (last_states.plant_lights != plant_lights.isOn()) {
    sml_plant_lights.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_plant_lights);
    last_states.plant_lights = plant_lights.isOn();
    sml_plant_lights.set(last_states.plant_lights);
  }
  if (last_states.air_mover != air_mover.isOn()) {
    sml_air_mover.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_air_mover);
    last_states.air_mover = air_mover.isOn();
    sml_air_mover.set(last_states.air_mover);
  }
  if (last_states.r3 != r3.isOn()) {
    sml_r3.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_r3);
    last_states.r3 = r3.isOn();
    sml_r3.set(last_states.r3);
  }
  if (last_states.r4 != r4.isOn()) {
    sml_r4.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_r4);
    last_states.r4 = r4.isOn();
    sml_r4.set(last_states.r4);
  }
  if (last_states.r5 != r5.isOn()) {
    sml_r5.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_r5);
    last_states.r5 = r5.isOn();
    sml_r5.set(last_states.r5);
  }
  if (last_states.r6 != r6.isOn()) {
    sml_r6.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_r6);
    last_states.r6 = r6.isOn();
    sml_r6.set(last_states.r6);
  }
  if (last_states.r7 != r7.isOn()) {
    sml_r7.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_r7);
    last_states.r7 = r7.isOn();
    sml_r7.set(last_states.r7);
  }
  if (last_states.fan0 != fan0.isOn()) {
    sml_fan0.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_fan0);
    last_states.fan0 = fan0.isOn();
    sml_fan0.set(last_states.fan0);
  }
  if (last_states.fan1 != fan1.isOn()) {
    sml_fan1.setUnit(SENML_UNIT_RATIO);
    state.add(&sml_fan1);
    last_states.fan1 = fan1.isOn();
    sml_fan1.set(last_states.fan1);
  }

  if (state.getFirst() != nullptr) {
#if !defined(DISABLE_SERIAL_DEBUG) || !defined(DISABLE_NET)
    StringStream sml_string_stream;
    state.toJson(&sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
    Serial.print(MQTT_TOPIC_OUT_ACTIONS " ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
    Serial.println(mqtt.publish(MQTT_TOPIC_OUT_ACTIONS, sml_string_stream.str()));
#endif
  }
}

sensors_event_t sensor_event;

void createAndSendBoxSensorMessage(Task *me) {
  bme_temp->getEvent(&sensor_event);
  box_temperature.set(sensor_event.temperature);

  bme_pressure->getEvent(&sensor_event);
  box_pressure.set(sensor_event.pressure * 100); // read unit is hPa (*100 to get Pa)

  bme_humidity->getEvent(&sensor_event);
  box_humidity.set(sensor_event.relative_humidity);

#if !defined(DISABLE_SERIAL_DEBUG) || !defined(DISABLE_NET)
  StringStream sml_string_stream;
  box.toJson(&sml_string_stream);
#endif
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(MQTT_TOPIC_OUT_SENSORS " ");
  Serial.print(millis());
  Serial.print(" ");
  Serial.println(sml_string_stream.str());
#endif
#ifndef DISABLE_NET
  Serial.println(mqtt.publish(MQTT_TOPIC_OUT_SENSORS, sml_string_stream.str()));
#endif
}

void runMqttLoop(Task *me) {
#ifndef DISABLE_NET
  if (!mqtt.connected()) {
    connectMqttClient();
  }

  mqtt.loop();
#endif
}

bool startMixNutrients(Task *me) {
  Serial.println("start mixing");
  for (auto &mixer : mixers) {
    mixer->on();
  }
  return true;
}

bool stopMixNutrients(Task *me) {
  Serial.println("stop mixing");
  for (auto &mixer : mixers) {
    mixer->off();
  }
  return true;
}

bool startDossing(Task *me) {
  Serial.println("dossing");
  // TODO:
  SoftTimer.add(&th_check_dossing_state);
  return true;
}

bool startPhBalancing(Task *me) {
  Serial.println("balancing ph starting");
  SoftTimer.add(&th_balance_ph);
  return true;
}

void dossingBalanceBasinPh(Task *me) {
  Serial.println("balancing ph");
}

bool stopPhBalancing(Task *me) {
  Serial.println("balancing ph finished");
  SoftTimer.remove(&th_balance_ph);
  return true;
}

bool flushReservoirToBasin(Task *me) {
  Serial.println("flushing");
  // TODO:
  return true;
}

void checkIfOffablesShouldOff(Task *me) {
  pump_flora_micro.checkShouldOff();
  pump_flora_gro.checkShouldOff();
  pump_flora_bloom.checkShouldOff();
  pump_ph_up_reservoir.checkShouldOff();
  pump_ph_up_basin.checkShouldOff();
  pump_ph_down_basin.checkShouldOff();

  submersible_pump.checkShouldOff();
  plant_lights.checkShouldOff();
  air_mover.checkShouldOff();
  r3.checkShouldOff();
  r4.checkShouldOff();
  r5.checkShouldOff();
  r6.checkShouldOff();
  r7.checkShouldOff();

  fan0.checkShouldOff();
  fan1.checkShouldOff();

#if defined(BUTTON_TOGGLE_CALIBRATOR)
  if (button_pump != nullptr && !button_pump->isOn()) {
    is_dosing = false;
  }
#endif
}

void dossingCheckState(Task *me) {
  if (is_dosing && !pump_flora_micro.isOn() && !pump_flora_gro.isOn() && !pump_flora_bloom.isOn()) {
    Serial.println("finished dossing");
    is_dosing = false;
    SoftTimer.remove(&th_check_dossing_state);
    th_start_ph_balancing.startDelayed();
  }
}

#if defined(BUTTON_HOLD_ON_MIXERS)

void buttonPressed() {
  for (auto &mixer : mixers) {
    mixer->on();
  }
}

void buttonReleased(unsigned long pressTimespan) {
  for (auto &mixer : mixers) {
    mixer->off();
  }
}

#elif defined(BUTTON_HOLD_ON_PUMPS)

void buttonPressed() {
  if (button_pump != nullptr) {
    if (!button_pump->isOn()) {
      button_pump->on();
    }
  }
}

void buttonReleased(unsigned long pressTimespan) {
  if (button_pump != nullptr) {
    if (button_pump->isOn()) {
      button_pump->off();
    }
  }
}

#elif defined(BUTTON_TOGGLE_ON)

void buttonPressed() {
  if (button_pump->isOn()) {
    button_pump->off();
  } else {
    button_pump->on();
  }
}

void buttonReleased(unsigned long pressTimespan) {}

#elif defined(BUTTON_TOGGLE_CALIBRATOR)

void buttonPressed() {
  if (!is_dosing) {
    is_dosing = true;

    const double WaterVolumeInMl = 15160.0;
    const double DosePerGallon = 7.5;
    double dose_amount, dose_time;

    // This should yield almost exactly 30mL
    dose_amount = WaterVolumeInMl * DosePerGallon / MillilitersPerUSGallon;
    dose_time = dose_amount / MilliliterToMilliseconds;

    Serial.print(dose_amount, 0);
    Serial.println(" mL");
    Serial.println(dose_time, 8);

    button_pump->on((unsigned long) (round(dose_time)));
  }
}

void buttonReleased(unsigned long pressTimespan) {}

#elif defined(BUTTON_START_DOSE)

void buttonPressed() {
  if (!is_dosing) {
    is_dosing = true;

    th_start_mix_nutrients.startDelayed();
  }
}

void buttonReleased(unsigned long pressTimespan) {}

#endif