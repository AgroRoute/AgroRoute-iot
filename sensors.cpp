#include "sensors.h"
#include <DHTesp.h>

static DHTesp dht;

void initSensors() {
    dht.setup(12, DHTesp::DHT22);
}

SensorData readSensors() {
    SensorData d;
    auto dh = dht.getTempAndHumidity();
    d.temp = dh.temperature;
    d.hum  = dh.humidity;
    return d;
}
