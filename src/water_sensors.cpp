#include <water_sensors.h>
#include <df_robot_probes.h>
#include <pin_assignment.h>
#include <mqtt_publish.h>
#include <constants/mqtt.h>

#include <EzoBoard.h>
#include <network.h>
#include <debug.h>


GravityTDS tds(PIN_TDS_SENSOR);

EzoBoard ezo_ph_basin(10, "PH Basin");
EzoBoard ezo_ec_basin(12, "EC Basin");
EzoBoard ezo_temp_basin(13, "Temp Basin");
EzoBoard ezo_ph_reservoir(11, "PH Reservoir");

void requestWaterSensorReadings(Task *me);
Task task_request_sensor_readings(30000, requestWaterSensorReadings);

void receiveAtlasSensorReadings(Task *me);
Task task_receive_sensor_readings(100, receiveAtlasSensorReadings);

bool createAndSendWaterSensorsMessage(Task *me);
DelayRun task_create_and_send_water_sensor_message(0, createAndSendWaterSensorsMessage);

void setupAndCheckSensorHardware() {
  FUNC_IN
  tds.setKvalueAddress(0xFF);
  FUNC_OUT
}

SenMLPack pack_sensors("urn:ctrl:sensor:");
SenMLFloatRecord sml_ph_basin("basin:ph", SENML_UNIT_PH);
SenMLFloatRecord sml_ec_basin("basin:ec", SENML_UNIT_SIEMENS);
SenMLFloatRecord sml_temp_basin("basin:" SENML_NAME_TEMPERATURE, SENML_UNIT_DEGREES_CELSIUS);
SenMLFloatRecord sml_ph_reservoir("reservoir:ph", SENML_UNIT_PH);
SenMLFloatRecord sml_ec_reservoir("reservoir:ec", SENML_UNIT_SIEMENS);
SenMLFloatRecord sml_temp_reservoir("reservoir:" SENML_NAME_TEMPERATURE, SENML_UNIT_SIEMENS);

static float basin_last_temperature = NAN;

void requestWaterSensorReadings(Task *me) {
  FUNC_IN

  // If we don't have compensation yet, don't worry about it
  if (basin_last_temperature != NAN) {
    FUNC(false, "sendReadWithTempComp ")
    ezo_ec_basin.sendReadWithTempComp(basin_last_temperature);
  }

  FUNC(false, "sendReadCmd ")
  ezo_ph_basin.sendReadCmd();
  FUNC(false, "sendReadCmd ")
  ezo_temp_basin.sendReadCmd();
  FUNC(false, "sendReadCmd ")
  ezo_ph_reservoir.sendReadCmd(); // TODO: Only care about this sensor when dossing

  FUNC(false, "add ")
  SoftTimer.add(&task_receive_sensor_readings);
  FUNC_OUT
}

void printFailureOnSensor(EzoBoard &sensor) {
  FUNC_IN
  Serial.print("Failed reading sensor ");
  Serial.println(sensor.getName());
  FUNC_OUT
}

bool checkRequest(EzoBoard &board) {
  FUNC_IN
  if (board.isRequestPending()) {
    EzoBoard::Error status = board.receiveReadCmd();
    if (status == EzoBoard::FAIL) {
      printFailureOnSensor(board);
    }
    FUNC_OUT
    return status != EzoBoard::NOT_READY;
  }

  FUNC_OUT
  return true;
}

void receiveAtlasSensorReadings(Task *me) {
  FUNC_IN
  bool finished = true;

  finished &= checkRequest(ezo_ph_basin);
  finished &= checkRequest(ezo_temp_basin);
  finished &= checkRequest(ezo_ec_basin);
  finished &= checkRequest(ezo_ph_reservoir);

  if (finished) {
    SoftTimer.remove(&task_receive_sensor_readings);
    task_create_and_send_water_sensor_message.startDelayed();
  }
  FUNC_OUT
}

void addIfNotError(EzoBoard &board, SenMLFloatRecord &record) {
  FUNC_IN
  if (board.getError() == EzoBoard::SUCCESS) {
    pack_sensors.add(record);
    record.set(board.getLastReceivedReading());
  }
  FUNC_OUT
}

bool createAndSendWaterSensorsMessage(Task *me) {
  FUNC_IN
  pack_sensors.clear();

  addIfNotError(ezo_ph_basin, sml_ph_basin);
  addIfNotError(ezo_temp_basin, sml_temp_basin);
  addIfNotError(ezo_ph_reservoir, sml_ph_reservoir);
  addIfNotError(ezo_ec_basin, sml_ec_basin);

  float res_temp = getReservoirTemp();
  pack_sensors.add(sml_temp_reservoir);
  sml_temp_reservoir.set(res_temp);

  tds.setTemperature(res_temp);
  pack_sensors.add(sml_ec_reservoir);
  sml_ec_reservoir.set(tds.getEcValue());

  // Necessary for the ec meter
  basin_last_temperature = sml_temp_basin.get();

  printAndPublish(MQTT_TOPIC_OUT_BASIN, pack_sensors);
  FUNC_OUT
  return true;
}
