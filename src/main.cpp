#include <Arduino.h>
#include "DHT.h"
#include "SPIFFS.h"
#include <U8g2lib.h>
#include "Update.h"
#include "WiFi.h"
#include <Wire.h>

#include "../lib/config.h"

#define DHTPIN 23
#define DHTTYPE DHT11
#define IS_BRIDGE true

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
DHT dht(DHTPIN, DHTTYPE);

int nodes = 0;

void updateDisplay(int temperature, int humidity)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tr);
  if (IS_BRIDGE && WiFi.status() == WL_CONNECTED)
  {
    u8g2.drawStr(0, 16, "wifi");
  }
  u8g2.drawStr(40, 16, (String(nodes) + " nodes").c_str());
  u8g2.drawStr(0, 32, ("H " + String(humidity) + "% T " + String(temperature) + "C").c_str());
  u8g2.sendBuffer();
}

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso28_tr);
  u8g2.drawStr(8, 29, "WETNET");
  u8g2.sendBuffer();

  dht.begin();

  initWiFi();
}

void loop()
{
  delay(2000);

  int h = int(dht.readHumidity());
  int t = int(dht.readTemperature());

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  updateDisplay(t, h);
}