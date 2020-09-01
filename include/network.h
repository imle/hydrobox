#ifndef HPCC_NETWORK_H
#define HPCC_NETWORK_H

//#define USE_ETHERNET
#define USE_WIFI defined(ARDUINO_AVR_UNO_WIFI_REV2) && !defined(USE_ETHERNET)

#if USE_WIFI
#include <WiFiNINA.h>
#else
#include <Ethernet.h>
#endif
#include <SPI.h> // Must include in main file for WiFiNINA

#include <Task.h>
#include <thingsml.h>
#include <PubSubClient.h>
#include <secret/wifi.h>
#include <secret/mqtt.h>
#include <constants/mqtt.h>


//#define DISABLE_NET
//#define DISABLE_SERIAL_DEBUG

extern char ssid[];
extern char pass[];

#if USE_WIFI
extern WiFiClient network;
#else
extern byte mac[];

extern EthernetClient network;
#endif

// Runs often to check for incoming mqtt messages and send those ready for transmission
extern Task th_run_mqtt_loop;
extern Task th_run_mqtt_reconnect_check;

void mqttCallback(char *topic, byte *payload, unsigned int length);
extern PubSubClient mqtt;

void printAndPublish(const String& topic, SenMLPack &pack);

void printWifiData();
void printCurrentNet();
void printMacAddress(byte mac[]);

#endif //HPCC_NETWORK_H
