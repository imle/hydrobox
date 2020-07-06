#ifndef HPCC_CONFIG_H
#define HPCC_CONFIG_H

#include <Client.h>
#include <HttpClient.h>

class Config {
 protected:
  HttpClient *client;

 public:
  Config(Client& client);

  void registerConfigValue(const String &key, const String & value);
  void registerConfigValue(const String &key, const char value[]);
  void registerConfigValue(const String &key, char value);
  void registerConfigValue(const String &key, unsigned char value);
  void registerConfigValue(const String &key, int value);
  void registerConfigValue(const String &key, unsigned int value);
  void registerConfigValue(const String &key, long value);
  void registerConfigValue(const String &key, unsigned long value);
  void registerConfigValue(const String &key, float value);
  void registerConfigValue(const String &key, double value);

  template<typename ConfigValue>
  ConfigValue RetrieveConfigValue(const String &key);

  String deviceId;
};

#endif //HPCC_CONFIG_H
