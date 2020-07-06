#include "../include/Config.h"


Config::Config(Client &client) {
  this->client = new HttpClient(client, "mainflux.imle.io", 443);
}

void Config::registerConfigValue(const String &key, const String &value) {
  this->registerConfigValue(key, value.c_str());
}

void Config::registerConfigValue(const String &key, char value) {
  this->registerConfigValue(key, &value);
}

void Config::registerConfigValue(const String &key, unsigned char value) {
  this->registerConfigValue(key, (char *) &value);
}

void Config::registerConfigValue(const String &key, int value) {
  this->registerConfigValue(key, String(value).c_str());
}

void Config::registerConfigValue(const String &key, unsigned int value) {
  this->registerConfigValue(key, String(value).c_str());
}

void Config::registerConfigValue(const String &key, long value) {
  this->registerConfigValue(key, String(value).c_str());
}

void Config::registerConfigValue(const String &key, unsigned long value) {
  this->registerConfigValue(key, String(value).c_str());
}

void Config::registerConfigValue(const String &key, float value) {
  this->registerConfigValue(key, String(value).c_str());
}

void Config::registerConfigValue(const String &key, double value) {
  this->registerConfigValue(key, String(value).c_str());
}

void Config::registerConfigValue(const String &key, const char value[]) {
  this->client->post(
      "/things/" + this->deviceId + "/config/" + key,
      "application/json",
      value);
}

template<>
String Config::RetrieveConfigValue<String>(const String &key) {
  this->client->get("/things/" + this->deviceId + "/config/" + key);
  return String(this->client->responseBody());
}

template<>
char Config::RetrieveConfigValue<char>(const String &key) {
  return RetrieveConfigValue<String>(key)[0];
}

template<>
unsigned char Config::RetrieveConfigValue<unsigned char>(const String &key) {
  return RetrieveConfigValue<String>(key)[0];
}

template<>
int Config::RetrieveConfigValue<int>(const String &key) {
  return RetrieveConfigValue<String>(key).toInt();
}

template<>
unsigned int Config::RetrieveConfigValue<unsigned int>(const String &key) {
  return RetrieveConfigValue<String>(key).toInt();
}

template<>
long Config::RetrieveConfigValue<long>(const String &key) {
  return RetrieveConfigValue<String>(key).toInt();
}

template<>
unsigned long Config::RetrieveConfigValue<unsigned long>(const String &key) {
  return RetrieveConfigValue<String>(key).toInt();
}

template<>
float Config::RetrieveConfigValue<float>(const String &key) {
  return RetrieveConfigValue<String>(key).toFloat();
}

template<>
double Config::RetrieveConfigValue<double>(const String &key) {
  return RetrieveConfigValue<String>(key).toDouble();
}

