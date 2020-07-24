#include <water_sensors.h>
#include <df.h>

#include <network.h>
#include <EzoBoard.h>
#include <StringStream.h>
#include <constants/mqtt.h>


GravityTDS tds(PIN_TDS_SENSOR);

EzoBoard ezo_ph_basin(10, "PH Basin");
EzoBoard ezo_ec_basin(12, "EC Basin");
EzoBoard ezo_temp_basin(13, "Temp Basin");
EzoBoard ezo_ph_reservoir(11, "PH Reservoir");

Task th_request_sensor_readings(30000, requestWaterSensorReadings);
Task th_receive_sensor_readings(100, receiveAtlasSensorReadings);
DelayRun th_create_and_send_water_sensor_message(0, createAndSendWaterSensorsMessage);

SenMLPack sensors(SENML_BASE_NAME_SENSORS);
SenMLFloatRecord sensors_ph_basin(F("basin:ph"), SENML_UNIT_PH);
SenMLFloatRecord sensors_ec_basin(F("basin:ec"), SENML_UNIT_SIEMENS);
SenMLFloatRecord sensors_temp_basin(F("basin:" SENML_NAME_TEMPERATURE), SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord sensors_ph_reservoir(F("reservoir:ph"), SENML_UNIT_PH);
SenMLFloatRecord sensors_ec_reservoir(F("reservoir:ec"), SENML_UNIT_SIEMENS);
SenMLFloatRecord sensors_temp_reservoir(F("reservoir:" SENML_NAME_TEMPERATURE), SENML_UNIT_SIEMENS);

static float basin_last_temperature = NAN;

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
    sensors.add(sensors_ph_basin);
    sensors_ph_basin.set(ezo_ph_basin.getLastReceivedReading());
  }
  if (ezo_temp_basin.getError() == EzoBoard::SUCCESS) {
    sensors.add(sensors_temp_basin);
    sensors_temp_basin.set(ezo_temp_basin.getLastReceivedReading());
  }
  if (ezo_ph_reservoir.getError() == EzoBoard::SUCCESS) {
    sensors.add(sensors_ph_reservoir);
    sensors_ph_reservoir.set(ezo_ph_reservoir.getLastReceivedReading());
  }
  if (ezo_ec_basin.getError() == EzoBoard::SUCCESS) {
    sensors.add(sensors_ec_basin);
    sensors_ec_basin.set(ezo_ec_basin.getLastReceivedReading());
  }

  float res_temp = getReservoirTemp();
  sensors.add(sensors_temp_reservoir);
  sensors_temp_reservoir.set(res_temp);
  tds.setTemperature(res_temp);
  sensors.add(sensors_ec_reservoir);
  sensors_ec_reservoir.set(tds.getEcValue());

  // Necessary for the ec meter
  basin_last_temperature = sensors_temp_basin.get();

#if !defined(DISABLE_SERIAL_DEBUG) || !defined(DISABLE_NET)
  StringStream sml_string_stream;
  sensors.toJson(sml_string_stream);
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
