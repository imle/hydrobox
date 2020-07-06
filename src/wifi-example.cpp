//#include <SPI.h>
//#include <WiFiNINA.h>
//#include <HttpClient.h>
//
//#include <Config.h>
//
//#include "../secret/wifi.h"
//
//char ssid[] = SECRET_SSID;    // your network SSID (name)
//char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
//
//char serverAddress[] = "192.168.0.3";  // server address
//int port = 8080;
//
//WiFiSSLClient client;
//Config config(client);
//
//uint8_t status = WL_IDLE_STATUS;
//
//void setup() {
//  Serial.begin(9600);
//  while (!Serial) {}
//
//  if (WiFi.status() == WL_NO_MODULE) {
//    Serial.println("Communication with WiFi module failed!");
//    while (true);
//  }
//
//  String fv = WiFiClass::firmwareVersion();
//  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
//    Serial.println("Please upgrade the firmware");
//  }
//
//  Serial.print("Attempting to connect to SSID: ");
//  Serial.println(ssid);
//  while (status != WL_CONNECTED) {
//    status = WiFi.begin(ssid, pass);
//    delay(10000);
//  }
//  Serial.println("Connected to wifi");
//}
//
//void loop() {
//
//}
