#include <network.h>


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

bool first_connect = true;
unsigned long time_of_begin = 0;

void connectMqttClient() {
  if (!wifi.connected()) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (status != WL_CONNECTED) {
      if (millis() > time_of_begin + 10000) { // At 10 seconds call begin again
        time_of_begin = millis();
        status = WiFi.begin(ssid, pass);
      }

      delay(1000); // Check for connection every second
    }
    Serial.println("Connected to wifi");
  }

  if (!mqtt.connected()) {
    if (!first_connect) {
      Serial.print("MQTT State: ");
      Serial.print(mqtt.state());
      Serial.print(" => ");
      switch (mqtt.state()) {
        case MQTT_CONNECTION_TIMEOUT: Serial.println("MQTT_CONNECTION_TIMEOUT"); break;
        case MQTT_CONNECTION_LOST: Serial.println("MQTT_CONNECTION_LOST"); break;
        case MQTT_CONNECT_FAILED: Serial.println("MQTT_CONNECT_FAILED"); break;
        case MQTT_DISCONNECTED: Serial.println("MQTT_DISCONNECTED"); break;
        case MQTT_CONNECTED: Serial.println("MQTT_CONNECTED"); break;
        case MQTT_CONNECT_BAD_PROTOCOL: Serial.println("MQTT_CONNECT_BAD_PROTOCOL"); break;
        case MQTT_CONNECT_BAD_CLIENT_ID: Serial.println("MQTT_CONNECT_BAD_CLIENT_ID"); break;
        case MQTT_CONNECT_UNAVAILABLE: Serial.println("MQTT_CONNECT_UNAVAILABLE"); break;
        case MQTT_CONNECT_BAD_CREDENTIALS: Serial.println("MQTT_CONNECT_BAD_CREDENTIALS"); break;
        case MQTT_CONNECT_UNAUTHORIZED: Serial.println("MQTT_CONNECT_UNAUTHORIZED"); break;
      }
    }

    Serial.print("Attempting to connect to MQTT server at: ");
    Serial.println(mqtt_server);
    if (mqtt.connect(SECRET_MQTT_CLIENT_ID, SECRET_MQTT_USER, SECRET_MQTT_PASS)) {
      mqtt.subscribe(MQTT_TOPIC_IN_COMMAND);
      mqtt.subscribe(MQTT_TOPIC_IN_STATUS);
    }
  } else {
    Serial.println("Connected to mqtt server.");
  }

  first_connect = false;
}

void runMqttLoop(Task *me) {
#ifndef DISABLE_NET
  if (!mqtt.connected()) {
    connectMqttClient();
  }

  mqtt.loop();
#endif
}