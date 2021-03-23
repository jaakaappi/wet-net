#include <Arduino.h>
#ifdef ESP32
#include "SPIFFS.h"
#include "Update.h"
#include "WiFi.h"
#endif
#ifdef ESP8266
#include "ESP8266WiFi.h"
#include "LittleFS.h"
#endif

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);                     // wait for a second
}