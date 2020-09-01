#ifndef HPCC_NETWORK_H
#define HPCC_NETWORK_H

//#define DISABLE_NET
//#define DISABLE_SERIAL_DEBUG

//#define USE_ETHERNET
#if defined(ARDUINO_AVR_UNO_WIFI_REV2) && !defined(USE_ETHERNET)
#define USE_WIFI
#endif

#include <SPI.h>
#ifdef USE_WIFI
#include <WiFiNINA.h>
#include <secret/wifi.h>
#else
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#endif

#include <Task.h>
#include <thingsml.h>

#ifdef USE_WIFI
extern char ssid[];
extern char pass[];

extern WiFiClient network;
extern uint8_t status;
#else
extern byte mac[];

extern EthernetClient network;
#endif

extern Task task_check_network_connectivity;

bool checkNetworkHardware();
void connectToNetwork();
void printNetworkInfo();

unsigned long getTime();

#endif //HPCC_NETWORK_H
