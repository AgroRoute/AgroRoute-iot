#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
struct SensorData {
    float temp;
    float hum;
};

void initSensors();
SensorData readSensors();

#endif
