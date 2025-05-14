#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include "sensors.h"

void initWiFi(const char* ssid, const char* pass);
void sendData(const SensorData &s, double lat, double lon);

#endif
