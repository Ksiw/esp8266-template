#include <Arduino.h>
#include "defines.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifi.h"
#include "main_process.h"

static WiFiClient espClient;
// extern WiFiClient espClient;
static PubSubClient client(espClient);
static const char *mqtt_server = MQTT_SERVER;
static const int mqtt_port = MQTT_PORT;
static uint32_t mqtt_next_connect_ms = 0;
static bool f_try_connect = true;

static void check_timeout_reconnect();
//-------------------------------------------------------------------------------

static void mqttConnect()
{
  if (!get_wifi_connect_status())
    return;

  if (!f_try_connect)
    return;

  client.setServer(mqtt_server, mqtt_port);
  Serial.println("Подключаюсь к MQTT...");
  if (!client.connected())
  {
    if (client.connect(MQTT_ID, MQTT_USER, MQTT_PASS))
    {
      Serial.println("Подключение успешно");

      client.setCallback([](char *topic, byte *payload, unsigned int length)
                         { parce_incoming_command(topic, payload, length); });
      client.subscribe(TOPIC_COMAND);
    }
    else
    {
      Serial.print("Не подключено. Код ошибки: ");
      Serial.println(client.state());
    }
  }
  f_try_connect = false;
}
//-------------------------------------------------------------------------------

void mqtt_process()
{
  check_timeout_reconnect();
  if (!client.connected())
  {
    mqttConnect();
  }
  client.loop();
}
//-------------------------------------------------------------------------------

void mqtt_init()
{
  mqttConnect();
}
//-------------------------------------------------------------------------------

static void check_timeout_reconnect()
{
  if (millis() > mqtt_next_connect_ms)
  {
    f_try_connect = true;
    mqtt_next_connect_ms = millis() + MQTT_RECONNECT_TIMEOUT;
  }
}
//-------------------------------------------------------------------------------

// example: mqttPrintf("topic", "Hello, %s! The answer is %d", "World", 42);
void mqttPrintf(const char *topic, const char *format, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  client.publish(topic, buffer);
}
//-------------------------------------------------------------------------------