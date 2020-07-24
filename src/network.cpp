#include "network.h"


extern char ssid[] = SECRET_SSID;
extern char pass[] = SECRET_PASS;

WiFiClient wifi;

String mqtt_server = "mqtt.imle.io";
uint8_t status = WL_IDLE_STATUS;

Task th_run_mqtt_loop(100, runMqttLoop);

PubSubClient mqtt(mqtt_server.c_str(), 1883, mqttCallback, wifi);

void mqttCallback(char *topic, byte *payload, unsigned int length) {
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(topic);
  Serial.print(" ");
  for (int i = 0; i < length; ++i) {
    Serial.print(payload[i]);
  }
  Serial.println();
#endif

  if (topic == MQTT_TOPIC_IN_STATUS) {
    // TODO:
  }
}

void connectMqttClient() {
  if (!wifi.connected()) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (status != WL_CONNECTED) {
      status = WiFi.begin(ssid, pass);
      delay(10000);
    }
    Serial.println("Connected to wifi");
  }

  Serial.print("Attempting to connect to MQTT server at: ");
  Serial.println(mqtt_server);
  if (mqtt.connect(SECRET_MQTT_CLIENT_ID)) {
    mqtt.subscribe(MQTT_TOPIC_IN_COMMAND);
    mqtt.subscribe(MQTT_TOPIC_IN_STATUS);
  }
  if (!mqtt.connected()) {
    Serial.println("Unable to authenticate against mqtt endpoint.");
  } else {
    Serial.println("Connected to mqtt server.");
  }
}

void runMqttLoop(Task *me) {
#ifndef DISABLE_NET
  if (!mqtt.connected()) {
    connectMqttClient();
  }

  mqtt.loop();
#endif
}