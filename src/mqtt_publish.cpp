#include <mqtt_publish.h>
#include <mqtt.h>
#include <debug.h>


#if !defined(DISABLE_NET)
const size_t BufferLen = 400;
char buffer[BufferLen];
#endif

void printAndPublish(const String &topic, SenMLPack &pack) {
  FUNC_IN
#if !defined(DISABLE_SERIAL_DEBUG)
  Serial.print(topic);
  Serial.print(" ");
  Serial.print(millis());
  Serial.print(" ");
#endif
#if !defined(DISABLE_NET)
  pack.toJson(buffer, BufferLen);
#endif
#if !defined(DISABLE_SERIAL_DEBUG) && !defined(DISABLE_NET)
  Serial.println(buffer);
#elif defined(DISABLE_SERIAL_DEBUG) && !defined(DISABLE_NET)
  // TODO?
#elif !defined(DISABLE_SERIAL_DEBUG) && defined(DISABLE_NET)
  pack.toJson(Serial);
  Serial.println();
#endif
#ifndef DISABLE_NET
  mqtt.publish(topic.c_str(), buffer);
#endif
  FUNC_OUT
}