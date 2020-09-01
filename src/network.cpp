#include <network.h>
#include <debug.h>


#ifdef USE_WIFI
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient network;
uint8_t status = WL_IDLE_STATUS;
#else
byte mac[] = {
    0xA8, 0x61, 0x0A, 0xAE, 0x6C, 0x70
};

EthernetClient network;

EthernetUDP ethernetUdp;
NTPClient timeClient(ethernetUdp);
#endif

#ifndef USE_WIFI
void sendNTPPacket(const char *address);
#endif

Task task_check_network_connectivity(2000, [](Task *me) {
  connectToNetwork();
#ifndef USE_WIFI
  timeClient.update();
#endif
});

void printMacAddress(byte mac[]);

unsigned long getTime() {
#ifdef USE_WIFI
  return WiFi.getTime();
#else
  return timeClient.getEpochTime();
#endif
}

#ifdef USE_WIFI
bool checkNetworkHardware() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    return false;
  }

  String fv = WiFiClass::firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  return true;
}

void connectToNetwork() {
  auto last_status = status;
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  if (last_status != status) {
    // you're connected now, so print out the data:
    Serial.print("Connected to SSID: ");
    Serial.println(ssid);
  }
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

void printCurrentWiFiNetwork() {
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

#else

bool checkNetworkHardware() {
  return true;
}

void connectToNetwork() {
  FUNC_IN
  if (network.connected()) {
    EthernetClass::maintain();

    FUNC_OUT
    return;
  }

  if (EthernetClass::begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");

    if (EthernetClass::hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.");
      while (true);
    }

    while (EthernetClass::linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected");
      delay(5000);
    }
  }

  timeClient.begin();

  FUNC_OUT
}

#endif

void printNetworkInfo() {
  FUNC_IN
#ifdef USE_WIFI
  printCurrentWiFiNetwork();
  printWifiData();
#else
  // TODO:
#endif

  FUNC_OUT
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
