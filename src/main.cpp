#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "DHT.h"
#include "SPIFFS.h"
#include "Update.h"
#include "WiFi.h"

#define DHTPIN 23
#define DHTTYPE DHT11

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso28_tr);
  u8g2.drawStr(8, 29, "MYBOTIC");
  u8g2.sendBuffer();

  dht.begin();

  Serial.begin(115200);
}

void loop()
{
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

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
}