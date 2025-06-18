#include <Arduino.h>
#include "sensors.h"
#include "network.h"

uint8_t PIN_FAN  = 17;
uint8_t PIN_HUM  = 4;
uint8_t PIN_HEAT = 15;


const char* WIFI_SSID  = "Wokwi-GUEST";
const char* WIFI_PASS  = "";
const char* DEVICE_ID  = "agro-route-001";
const char* API_KEY    = "test-api-key-123";


#define GPS_RX_PIN 21
#define GPS_TX_PIN 22

const unsigned long SEND_INTERVAL = 5000;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  delay(200);

  initSensors();

  pinMode(PIN_FAN, OUTPUT);   digitalWrite(PIN_FAN, LOW);
  pinMode(PIN_HUM, OUTPUT);   digitalWrite(PIN_HUM, LOW);
  pinMode(PIN_HEAT, OUTPUT);  digitalWrite(PIN_HEAT, LOW);

  initNetwork(WIFI_SSID, WIFI_PASS, DEVICE_ID, API_KEY);

  Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

double parseNmeaCoord(const String &field, const String &dir) {
  double v = field.toFloat();
  int deg = int(v / 100);
  double min = v - deg * 100.0;
  double coord = deg + min / 60.0;
  if (dir == "S" || dir == "W") coord = -coord;
  return coord;
}

void loop() {
  networkLoop();

  unsigned long now = millis();
  if (now - lastSend >= SEND_INTERVAL) {
    lastSend = now;

    SensorData sd = readSensors();

    double lat = NAN, lon = NAN;
    if (Serial2.available()) {
      String line = Serial2.readStringUntil('\n');
      if (line.startsWith("$GPGGA")) {
        std::vector<String> parts;
        int idx = 0;
        while (true) {
          int comma = line.indexOf(',', idx);
          if (comma < 0) break;
          parts.push_back(line.substring(idx, comma));
          idx = comma + 1;
        }
        lat = parseNmeaCoord(parts[2], parts[3]);
        lon = parseNmeaCoord(parts[4], parts[5]);
      }
    }

    sendMonitoringData(sd, lat, lon);
  }

  delay(10);
}
