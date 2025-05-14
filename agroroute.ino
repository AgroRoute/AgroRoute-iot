#include <Arduino.h>
#include "sensors.h"
#include "network.h"


// UART2 pins (RX2=21, TX2=22)
#define GPS_RX_PIN 21
#define GPS_TX_PIN 22

const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("=== AgroRoute IoT Demo ===");

    initSensors();
    Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    initWiFi(WIFI_SSID, WIFI_PASSWORD);
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
    SensorData sd = readSensors();

    double lat = NAN, lon = NAN;
    if (Serial2.available()) {
        String line = Serial2.readStringUntil('\n');
        if (line.startsWith("$GPGGA")) {
            auto parts = std::vector<String>();
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

    sendData(sd, lat, lon);
    delay(5000);
}

