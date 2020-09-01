#ifndef HPCC_MQTT_H
#define HPCC_MQTT_H

#include <PubSubClient.h>
#include <Task.h>

extern PubSubClient mqtt;

extern Task task_check_mqtt_connectivity;
extern Task task_mqtt_loop;

#endif //HPCC_MQTT_H
