#include <water_sensors.h>
#include <df.h>

#include <network.h>
#include <EzoBoard.h>
#include <constants/mqtt.h>
#include <pin_assignment.h>


GravityTDS tds(PIN_TDS_SENSOR);

EzoBoard ezo_ph_basin(10, "PH Basin");
EzoBoard ezo_ec_basin(12, "EC Basin");
EzoBoard ezo_temp_basin(13, "Temp Basin");
EzoBoard ezo_ph_reservoir(11, "PH Reservoir");

Task th_request_sensor_readings(30000, requestWaterSensorReadings);
Task th_receive_sensor_readings(100, receiveAtlasSensorReadings);
DelayRun th_create_and_send_water_sensor_message(0, createAndSendWaterSensorsMessage);

SenMLPack sensors(SENML_BASE_NAME_SENSORS);
SenMLFloatRecord sensors_ph_basin("basin:ph", SENML_UNIT_PH);
SenMLFloatRecord sensors_ec_basin("basin:ec", SENML_UNIT_SIEMENS);
SenMLFloatRecord sensors_temp_basin("basin:" SENML_NAME_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord sensors_ph_reservoir("reservoir:ph", SENML_UNIT_PH);
SenMLFloatRecord sensors_ec_reservoir("reservoir:ec", SENML_UNIT_SIEMENS);
SenMLFloatRecord sensors_temp_reservoir("reservoir:" SENML_NAME_TEMPERATURE, SENML_UNIT_SIEMENS);

static float basin_last_temperature = NAN;

void requestWaterSensorReadings(Task *me) {
  // If we don't have compensation yet, don't worry about it
  if (basin_last_temperature != NAN) {
    ezo_ec_basin.sendReadWithTempComp(basin_last_temperature);
  }

  ezo_ph_basin.sendReadCmd();
  ezo_temp_basin.sendReadCmd();
  ezo_ph_reservoir.sendReadCmd(); // TODO: Only care about this sensor when dossing

  SoftTimer.add(&th_receive_sensor_readings);
}

void printFailureOnSensor(EzoBoard &sensor) {
  Serial.print("Failed reading sensor ");
  Serial.println(sensor.getName());
}

bool checkRequest(EzoBoard &board) {
  if (board.isRequestPending()) {
    EzoBoard::Error status = board.receiveReadCmd();
    if (status == EzoBoard::FAIL) {
      printFailureOnSensor(board);
    }
    return status != EzoBoard::NOT_READY;
  }

  return true;
}

void receiveAtlasSensorReadings(Task *me) {
  bool finished = true;

  finished &= checkRequest(ezo_ph_basin);
  finished &= checkRequest(ezo_temp_basin);
  finished &= checkRequest(ezo_ec_basin);
  finished &= checkRequest(ezo_ph_reservoir);

  if (finished) {
    SoftTimer.remove(&th_receive_sensor_readings);
    th_create_and_send_water_sensor_message.startDelayed();
  }
}

void addIfNotError(EzoBoard &board, SenMLFloatRecord &record) {
  if (board.getError() == EzoBoard::SUCCESS) {
    sensors.add(record);
    record.set(board.getLastReceivedReading());
  }
}

bool createAndSendWaterSensorsMessage(Task *me) {
  sensors.clear();

  addIfNotError(ezo_ph_basin, sensors_ph_basin);
  addIfNotError(ezo_temp_basin, sensors_temp_basin);
  addIfNotError(ezo_ph_reservoir, sensors_ph_reservoir);
  addIfNotError(ezo_ec_basin, sensors_ec_basin);

  float res_temp = getReservoirTemp();
  sensors.add(sensors_temp_reservoir);
  sensors_temp_reservoir.set(res_temp);

  tds.setTemperature(res_temp);
  sensors.add(sensors_ec_reservoir);
  sensors_ec_reservoir.set(tds.getEcValue());

  // Necessary for the ec meter
  basin_last_temperature = sensors_temp_basin.get();

  printAndPublish(MQTT_TOPIC_OUT_BASIN, sensors);
  return true;
}
