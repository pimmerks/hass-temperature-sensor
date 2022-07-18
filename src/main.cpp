#include <WiFi.h>

#include "config.h"
#include "temp.cpp"
#include "mqtt.cpp"

TempReader tempReader;
MQTT mqttClient(state_topic, mqtt_id, mqtt_server);

void setupWifi() {
  Serial.print("Connecting to wifi ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("My IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up...");
  setupWifi();

  mqttClient.ensureConnection();
  mqttClient.announceSensorToHomeAssistant();

  tempReader.begin();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection to wifi lost, reconnecting");
    setupWifi();
  }

  // Ensure we are connected to our mqtt server.
  mqttClient.ensureConnection();

  // We are connected to wifi and mqtt, let's read data and send it to our mqtt server
  SensorData data = tempReader.getData();

  mqttClient.publishSensorData(&data);
  delay(1000);
}
