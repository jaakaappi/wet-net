#include <Arduino.h>
#include "DHT.h"
#include <PubSubClient.h>
#include "SPIFFS.h"
#include <U8g2lib.h>
#include "Update.h"
#include <WiFiClient.h>
#include "WiFi.h"
#include <Wire.h>

#include "../lib/config.h"

#define DHTPIN 23
#define DHTTYPE DHT11
#define MEASUREMENT_INTERVAL_MS 60 * 1000

void mqttCallback(char *topic, byte *payload, unsigned int length);

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
IPAddress myIP(0, 0, 0, 0);
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wifiClient);

int temperature = 0;
int humidity = 0;
bool led_on = false;

void updateDisplay()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tr);
  if (WiFi.status() == WL_CONNECTED)
  {
    u8g2.drawStr(0, 16, "wifi");
  }
  if (mqttClient.connected())
  {
    u8g2.drawStr(40, 16, "mqtt");
  }
  u8g2.drawStr(0, 32, ("H " + String(humidity) + "% T " + String(temperature) + "C").c_str());
  u8g2.sendBuffer();
}

void startWifi()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(("Connecting to WIFI " + String(ssid)).c_str());
    WiFi.begin(ssid, password);
    delay(2000);
  }
  Serial.println("WIFI connected");
}

void startMqtt()
{
  while (!mqttClient.connected())
  {
    Serial.println("Connecting to MQTT");
    mqttClient.connect(thingsboardDeviceName, authtoken, nullptr);
    mqttClient.subscribe("v1/devices/me/rpc/request/+");
    delay(2000);
  }
  Serial.println("MQTT connected");
}

void setup()
{
  Serial.begin(115200);
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);

  u8g2.begin();
  updateDisplay();

  dht.begin();

  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(60);
  startWifi();
  startMqtt();
}

int dht_flag = 0;

void loop()
{
  mqttClient.loop();

  if (dht_flag <= millis())
  {
    dht_flag = millis() + MEASUREMENT_INTERVAL_MS;

    int h = int(dht.readHumidity());
    int t = int(dht.readTemperature());

    if (isnan(h) || isnan(t))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    temperature = t;
    humidity = h;

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
    updateDisplay();

    if (WiFi.status() == WL_CONNECTED && mqttClient.connected())
    {
      if (mqttClient.publish("v1/devices/me/telemetry", ("{\"humidity\":" + String(humidity) + ", \"temperature\":" + String(temperature) + "}").c_str()))
      {
        Serial.println("Message sent");
      }
      else
      {

        Serial.println("Message failed");
      }
    }
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    startWifi();
  }
  if (!mqttClient.connected())
  {
    startMqtt();
  }
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  Serial.println("mqtt message");
  Serial.println(topic);
  led_on = !led_on;
  if (led_on)
  {
    digitalWrite(18, HIGH);
  }
  else
  {
    digitalWrite(18, LOW);
  }
}
