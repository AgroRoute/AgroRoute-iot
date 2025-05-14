#include "network.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static const char* ENDPOINT  = "https://mundo.free.beeceptor.com/temperature";
static const char* DEVICE_ID = "HC2947";

void initWiFi(const char* ssid, const char* pass) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.print("Conectando Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println(WiFi.localIP().toString());
}

void sendData(const SensorData &s, double lat, double lon) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi desconectado");
        return;
    }

    HTTPClient http;
    http.begin(ENDPOINT);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    doc["deviceId"] = DEVICE_ID;
    auto j = doc.createNestedObject("dht22");
    j["temperature"] = s.temp;
    j["humidity"]    = s.hum;
    auto g = doc.createNestedObject("gps");
    g["lat"] = lat;
    g["lon"] = lon;

    String payload;
    serializeJson(doc, payload);

    Serial.print("Enviando payload: ");
    Serial.println(payload);

    int code = http.POST(payload);
    if (code > 0) {
        Serial.printf("POST %d\n", code);
    } else {
        Serial.printf("POST error: %s\n", http.errorToString(code).c_str());
    }
    http.end();
}
