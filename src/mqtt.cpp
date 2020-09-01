#include <mqtt.h>
#include <network.h>
#include <secret/mqtt.h>
#include <constants/mqtt.h>
#include <debug.h>


void mqttCallback(char *topic, byte *payload, unsigned int length);
PubSubClient mqtt(MQTT_SERVER, 1883, mqttCallback, network);

void checkMqttConnectivity(Task *me);
Task task_check_mqtt_connectivity(2000, checkMqttConnectivity);
Task task_mqtt_loop(100, [](Task *me) { mqtt.loop(); });

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  FUNC_IN
  Serial.print(topic);
  Serial.print(" ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  FUNC_OUT
}

void mqttOnConnect() {
  FUNC_IN
  if (!mqtt.setBufferSize(400)) {
    Serial.println("ERROR: Failed to set mqtt buffer size.");
  }

  mqtt.subscribe(MQTT_TOPIC_IN_COMMAND);
  mqtt.subscribe(MQTT_TOPIC_IN_STATUS);

  mqtt.publish(MQTT_TOPIC_OUT_STATUS, R"([{"n":"urn:ctrl:online","u":"//","vb":true}])", true);
  FUNC_OUT
}

void checkMqttConnectivity(Task *me) {
  FUNC_IN
  if (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt
        .connect("arduinoClient", MQTT_TOPIC_OUT_STATUS, 2, true, R"([{"n":"urn:ctrl:online","u":"//","vb":false}])")) {
      Serial.println("connected");

      mqttOnConnect();
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqtt.state());
    }
  }
  FUNC_OUT
}
