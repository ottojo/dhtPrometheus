#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "metric.h"

#define PORT 9578
#define SSID "JIoT"
#define PASSWORD "PW"

int lastMeasure = millis();
float lastTemperature = NAN;
float lastHumidity = NAN;

ESP8266WebServer server(PORT);

#define DHTPIN            D4
#define DHTTYPE           DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

Metric temp(MetricType::gauge, "dht_temperature", "todo help tmp", 1);

Metric hum(MetricType::gauge, "dht_humidity", "todo help humid", 1);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT22 Prometheus exporter");

  Serial.println("Initializing sensor");
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("°C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("°C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("°C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
  delayMS = sensor.min_delay / 1000;
  Serial.printf("Sensor min delay %dms\n", delayMS);

  Serial.println("Initializing WiFi");
  Serial.printf("Connectong to \"%s\"\n", SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/metrics", handleMetrics);
  server.begin();
  Serial.printf("HTTP server started on port %d\n", PORT);
}


void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

  if (millis() - lastMeasure > delayMS) {
    measure();
    lastMeasure = millis();
  }
}

void handleRoot() {
  server.send(200, "text/html", "<a href=\"/metrics\">metrics</a>");
}

void handleMetrics() {

  String response = "";
  response += temp.getString() + hum.getString();

  server.send(200, "text/plain; version=0.0.4", response);
}

void measure() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp.setValue(event.temperature);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(String(event.temperature, 1));
    Serial.println("°C");
  }
  dht.humidity().getEvent(&event);
  hum.setValue(event.relative_humidity);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(String(event.relative_humidity, 1));
    Serial.println("%");
  }
}
