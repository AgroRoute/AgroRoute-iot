#include "network.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

static const char* _deviceId = nullptr;
static const char* _apiKey   = nullptr;
static WiFiClient   _wifiClient;
static PubSubClient _mqttClient(_wifiClient);

static void _mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg, top = topic;
  for (unsigned i = 0; i < length; i++) msg += (char)payload[i];
  Serial.printf("[MQTT RX] topic=%s msg=%s\n", top.c_str(), msg.c_str());

  if (top.endsWith("/fan")) {
    digitalWrite(PIN_FAN, msg == "ON" ? HIGH : LOW);
    Serial.println("  → Fan_pin ahora " + String(msg));
  }
  else if (top.endsWith("/heater")) {
    digitalWrite(PIN_HEAT, msg == "ON" ? HIGH : LOW);
    Serial.println("  → Heat_pin ahora " + String(msg));
  }
  else if (top.endsWith("/humidifier")) {
    digitalWrite(PIN_HUM, msg == "ON" ? HIGH : LOW);
    Serial.println("  → Humid_pin ahora " + String(msg));
  }
}

static void _connectMQTT() {
  Serial.println("[MQTT] Conectando a broker " MQTT_BROKER " …");
  _mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  _mqttClient.setCallback(_mqttCallback);

  while (!_mqttClient.connected()) {
    Serial.print("[MQTT] Intentando conectar… ");
    int rc = _mqttClient.connect(_deviceId, "", "");
    if (rc == 1) {
      Serial.println("¡Conectado!");
      String topic = String("agroroute/") + _deviceId + "/#";
      _mqttClient.subscribe(topic.c_str());
      Serial.println("[MQTT] Suscrito a: " + topic);
    } else {
      Serial.printf("[MQTT] Error rc=%d, retrying in 2s\n", _mqttClient.state());
      delay(2000);
    }
  }
}

void initNetwork(const char* ssid, const char* pass, const char* deviceId, const char* apiKey) {
  _deviceId = deviceId;
  _apiKey   = apiKey;

  Serial.print("[WiFi] Conectando a ");
  Serial.print(ssid);
  Serial.print(" …");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n[WiFi] Conectado, IP: " + WiFi.localIP().toString());

  _connectMQTT();
}

void networkLoop() {
  if (!_mqttClient.connected()) {
    Serial.println("[MQTT] Desconectado, reconectando…");
    _connectMQTT();
  }
  _mqttClient.loop();
}

void sendMonitoringData(const SensorData &s, double lat, double lon) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[HTTP] WiFi desconectado, salto envío");
    return;
  }

  HTTPClient http;
  http.begin(ENDPOINT_MONITORING);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", _apiKey);

  StaticJsonDocument<256> doc;
  doc["device_id"]   = _deviceId;
  doc["temperature"] = s.temp;
  doc["humidity"]    = s.hum;
  doc["latitude"]    = lat;
  doc["longitude"]   = lon;

  String payload;
  serializeJson(doc, payload);
  Serial.println("[HTTP] POST payload: " + payload);

  int code = http.POST(payload);
  Serial.println("[HTTP] Response code: " + String(code));
  if (code > 0) {
    String resp = http.getString();
    Serial.println("[HTTP] Body: " + resp);
  } else {
    Serial.println("[HTTP] Error: " + http.errorToString(code));
  }
  http.end();
}