#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 23
#define DHTTYPE DHT11
#define SOIL_SENSOR_PIN 34

const char* ssid = "HUAWEI-778B";
const char* password = "QEBE30040TM";
const char* serverUrl = "https://apisapp-1.onrender.com/registerSensorData";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  delay(2000);
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float humedad_suelo = leerHumedadSuelo();

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String data = "{\"temperatura\": " + String(temperatura) + ", \"humedad\": " + String(humedad) + ", \"humedad_del_suelo\": " + String(humedad_suelo) + "}";
  Serial.print(humedad_suelo);
  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error in HTTP request: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  delay(60000); // Envía datos cada minuto
}

float leerHumedadSuelo() {
  int sensorValue = analogRead(SOIL_SENSOR_PIN);
  float humedad_suelo = map(sensorValue, 0, 1023, 0, 100); // Mapea el valor del sensor a un rango de 0 a 100
  return humedad_suelo;
}
