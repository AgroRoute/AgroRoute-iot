#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "sensors.h"


#define ENDPOINT_MONITORING  "http://38.25.66.132:8000/api/v1/monitoring/data-records"

#define MQTT_BROKER          "test.mosquitto.org"
#define MQTT_PORT            1883

extern uint8_t PIN_FAN;
extern uint8_t PIN_HUM;
extern uint8_t PIN_HEAT;

void initNetwork(const char* ssid, const char* pass, const char* deviceId, const char* apiKey);

void networkLoop();
void sendMonitoringData(const SensorData &s, double lat, double lon);


#endif
