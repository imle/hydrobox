#include <Wire.h>

#include <SoftTimer.h>

#include <button_actions.h>
#include <float_sensors.h>
#include <network.h>
#include <dossing.h>
#include <state.h>
#include <mqtt.h>
#include <sump.h>
#include <box.h>
#include <water_sensors.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Startup...");

  Wire.begin();


#ifndef DISABLE_NET
  if (!checkNetworkHardware()) {
    while (true);
  }

  connectToNetwork();
  printNetworkInfo();
#endif

  if (!setupAndCheckBoxHardware()) {
    while (true);
  }

  setupDossing();
  setupAndCheckSensorHardware();

#ifndef DISABLE_NET
  SoftTimer.add(&task_check_network_connectivity);
  SoftTimer.add(&task_check_mqtt_connectivity);
  SoftTimer.add(&task_mqtt_loop);
#endif
  SoftTimer.add(&task_check_pump_state);
  SoftTimer.add(&task_request_sensor_readings);
  SoftTimer.add(&task_publish_box_sensor_readings);
  SoftTimer.add(&task_check_state_changes_and_notify);

  setupButtonAction();
  setupFloatSensors();
}
