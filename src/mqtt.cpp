// homeassistant/sensor/esp32-woonkamer/<sensor_type>/config
// Sensor type: temperature, humidity, voc, co2, etc
// {
//     "device_class": "temperature",
//     "enabled_by_default": true,
//     "name": "Meterkast Temperatuur",
//     "state_class": "measurement",
//     "state_topic": "home/sensor/temp-meterkast",
//     "unique_id": "esp_temp_meterkast_temperature",
//     "unit_of_measurement": "°C",
//     "value_template": "{{ value_json.temperature }}"
// }

#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "SensorData.h"

class MQTT {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    const char* stateTopic;
    const char* id;

    void anounceSensor(const char* topic, String name, String uniqueId, String deviceClass, String unit, String templ) {
      StaticJsonDocument<400> doc;
      char output[400];

      doc["enabled_by_default"] = true;
      doc["state_class"] = "measurement";
      doc["device_class"] = deviceClass;
      doc["name"] = name;
      doc["unique_id"] = uniqueId;
      doc["unit_of_measurement"] = unit;
      doc["value_template"] = templ;
      doc["state_topic"] = MQTT::stateTopic;

      serializeJson(doc, output);
      delay(20);

      if (!mqttClient.publish(topic, output)) {
        Serial.println("Could not announce " + name);
      }
    }

  public:
    MQTT(const char* state_topic, const char* mqtt_id, const char* mqtt_server) : mqttClient(wifiClient)
    {
      stateTopic = state_topic;
      id = mqtt_id;
      mqttClient.setServer(mqtt_server, 1883);
      mqttClient.setBufferSize(500);
    }

    /**
     * Ensure a connection to the mqtt server is made.
     */
    void ensureConnection() {
      // Loop until we're reconnected
      while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(id)) {
          Serial.println("connected");
        } else {
          Serial.print("failed, rc=");
          Serial.println(mqttClient.state());
          delay(1000);
        }
      }
    }

    void announceSensorToHomeAssistant() {
      anounceSensor("homeassistant/sensor/woonesp32/temperature/config", "Woonkamer Temperatuur", "esp32_woonkamer_temp", "temperature", "°C", "{{ value_json.tempC }}");
      anounceSensor("homeassistant/sensor/woonesp32/humidity/config", "Woonkamer Luchtvochtigheid", "esp32_woonkamer_hum", "humidity", "%", "{{ value_json.humidity }}");
      anounceSensor("homeassistant/sensor/woonesp32/pressure/config", "Woonkamer Luchtdurk", "esp32_woonkamer_pressure", "pressure", "Pa", "{{ value_json.pressure }}");
      anounceSensor("homeassistant/sensor/woonesp32/co2/config", "Woonkamer CO2", "esp32_woonkamer_co2", "carbon_dioxide", "PPM", "{{ value_json.co2 }}");
      anounceSensor("homeassistant/sensor/woonesp32/tvoc/config", "Woonkamer TVOC", "esp32_woonkamer_tvoc", "volatile_organic_compounds", "PPB", "{{ value_json.tvoc }}");
    }

    double round2(double value) {
      return (int)(value * 100 + 0.5) / 100.0;
    }

    void publishSensorData(SensorData* sensorData) {
      StaticJsonDocument<128> doc;
      char output[128];

      doc["co2"]      = sensorData->co2;
      doc["tvoc"]     = sensorData->tvoc;
      doc["tempC"]    = round2(sensorData->tempC);
      doc["pressure"] = round2(sensorData->pressure);
      doc["altitude"] = round2(sensorData->altitude);
      doc["humidity"] = round2(sensorData->humidity);
      doc["error"]    = sensorData->error;

      serializeJson(doc, output);
      Serial.println(output);

      mqttClient.publish("esp32/woonkamer", output);
    }
};
