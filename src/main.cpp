#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#ifdef ESP32
#include "SPIFFS.h"
#include "Update.h"
#include "WiFi.h"
#endif
#ifdef ESP8266
#include "ESP8266WiFi.h"
#include "LittleFS.h"
#endif

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  u8g2.begin();
  u8g2.clearBuffer();                    // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr); // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(8, 29, "MYBOTIC");        // write something to the internal memory
  u8g2.sendBuffer();
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);                     // wait for a second
}