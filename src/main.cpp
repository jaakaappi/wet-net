#include <Arduino.h>
#include "DHT.h"
#include "painlessMesh.h"
#include "SPIFFS.h"
#include <U8g2lib.h>
#include "Update.h"
#include "WiFi.h"
#include <Wire.h>

#include "../lib/config.h"

#define MESH_PREFIX "wetnet"
#define MESH_PORT 5555

#define DHTPIN 23
#define DHTTYPE DHT11

#define IS_BRIDGE true

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

DHT dht(DHTPIN, DHTTYPE);

int nodes = 0;
Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

void updateDisplay(int temperature, int humidity)
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

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void sendMessage()
{
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast(msg);
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
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

  mesh.init(MESH_PREFIX, meshpassword, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onDroppedConnection(&droppedNodeCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

int dht_flag = 0;

void loop()
{
  if (dht_flag <= millis())
  {
    dht_flag = millis() + 2000;

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

  mesh.update();
}