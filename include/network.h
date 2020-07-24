#ifndef HPCC_NETWORK_H
#define HPCC_NETWORK_H

#include <Task.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "secret/wifi.h"
#include "secret/mqtt.h"
#include "constants/mqtt.h"


#define DISABLE_NET
//#define DISABLE_SERIAL_DEBUG

extern char ssid[];
extern char pass[];

extern WiFiClient wifi;

void connectMqttClient();

// Runs often to check for incoming mqtt messages and send those ready for transmission
void runMqttLoop(Task *me);
extern Task th_run_mqtt_loop;

void mqttCallback(char *topic, byte *payload, unsigned int length);
extern PubSubClient mqtt;

#endif //HPCC_NETWORK_H
