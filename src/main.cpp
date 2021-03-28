#include <Arduino.h>
#include "DHT.h"
#include "painlessMesh.h"
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

#define MESH_PREFIX "wetnet"
#define MESH_PORT 5555
#define IS_BRIDGE true
#define HOSTNAME "MQTT_Bridge"

void mqttCallback(char *topic, byte *payload, unsigned int length);
IPAddress getlocalIP();

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

DHT dht(DHTPIN, DHTTYPE);

int nodes = 0;
painlessMesh mesh;
WiFiClient wifiClient;
IPAddress myIP(0, 0, 0, 0);
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wifiClient);

int temperature = 0;
int humidity = 0;

void updateDisplay()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tr);
  if (IS_BRIDGE)
  {
    if (WiFi.status() == WL_CONNECTED)
    {

      u8g2.drawStr(0, 16, "wifi");
    }
    else
    {

      u8g2.drawStr(0, 16, "wait");
    }
  }
  u8g2.drawStr(40, 16, (String(nodes) + " nodes").c_str());
  u8g2.drawStr(0, 32, ("H " + String(humidity) + "% T " + String(temperature) + "C").c_str());
  u8g2.sendBuffer();
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  nodes++;
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void droppedNodeCallback(uint32_t nodeId)
{
  Serial.printf("Dropped connection %u\n", nodeId);
  nodes--;
}

void sendMQTTTElemetry()
{
  mqttClient.publish("v1/devices/me/telemetry", ("{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}").c_str());
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

  // if (IS_BRIDGE)
  // {
  //   mesh.setRoot(true);
  // }

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION); // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, meshpassword, MESH_PORT, WIFI_AP_STA);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onDroppedConnection(&droppedNodeCallback);

  mesh.stationManual(ssid, password);
  mesh.setHostname(HOSTNAME);

  mesh.setRoot(true);
  mesh.setContainsRoot(true);
}

int dht_flag = 0;

void loop()
{
  if (dht_flag <= millis())
  {
    dht_flag = millis() + 5000;

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
    sendMQTTTElemetry();
  }

  mesh.update();
  mqttClient.loop();

  if (myIP != getlocalIP())
  {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

    if (mqttClient.connect("painlessMeshClient", authtoken, nullptr))
    {
      Serial.println("MQTT ready");
    }
  }
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
}

IPAddress getlocalIP()
{
  return IPAddress(mesh.getStationIP());
}
