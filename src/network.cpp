#include <network.h>
#include <StringStream.h>


char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

#if USE_WIFI
WiFiClient network;
uint8_t status = WL_IDLE_STATUS;
#else
EthernetClient network;
#endif

PubSubClient mqtt(network);

Task th_run_mqtt_loop(100, [](Task *me) { mqtt.loop(); });

void mqttClientConnect(Task *me);
Task th_run_mqtt_reconnect_check(100, mqttClientConnect);

void mqttCallback(char *topic, byte *payload, unsigned int length) {
#ifndef DISABLE_SERIAL_DEBUG
  Serial.print(topic);
  Serial.print(" ");
  for (unsigned int i = 0; i < length; ++i) {
    Serial.print(payload[i]);
  }
  Serial.println();
#endif

  if (strcmp(topic, MQTT_TOPIC_IN_STATUS) == 0) {
    // TODO:
  }
}

bool first_connect = true;

void mqttClientConnect(Task *me) {
  if (!mqtt.connected()) {
    // Loop until we're reconnected
    while (!mqtt.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (mqtt.connect("arduinoClient")) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        mqtt.publish("outTopic", "hello world");
        // ... and resubscribe
        mqtt.subscribe("inTopic");
      } else {
        Serial.print("failed, rc=");
        Serial.print(mqtt.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}

char buffer[350];

void printAndPublish(const String &topic, SenMLPack &pack) {
#if !defined(DISABLE_SERIAL_DEBUG)
  Serial.print(topic);
  Serial.print(" ");
  Serial.print(millis());
  Serial.print(" ");
#endif
#if !defined(DISABLE_NET)
  pack.toJson(buffer, 350);
#endif
#if !defined(DISABLE_SERIAL_DEBUG) && !defined(DISABLE_NET)
  Serial.println(buffer);
#elif defined(DISABLE_SERIAL_DEBUG) && !defined(DISABLE_NET)
#elif !defined(DISABLE_SERIAL_DEBUG) && defined(DISABLE_NET)
  pack.toJson(Serial);
  Serial.println();
#endif
#ifndef DISABLE_NET
  mqtt.publish(topic.c_str(), buffer);
#endif
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
