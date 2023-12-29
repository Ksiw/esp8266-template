#include <ESP8266WiFi.h>
#include "defines.h"
#include "wifi.h"

#define CHECK_CONNECT_MS 100

static const char *ssid = WIFI_SSID;
static const char *password = WIFI_PASSWORD;

static WiFiEventHandler wifiConnectHandler;
static WiFiEventHandler wifiDisconnectHandler;

static uint32_t next_connect_ms = 0;
static bool f_init = true;
static bool f_start_connect = false;
//-------------------------------------------------------------------------------

static void onWiFiConnect(const WiFiEventStationModeGotIP &event)
{
  f_start_connect = false;
  WRITE_INFO("Wi-Fi соединение установлено.", "\n");
  WRITE_INFO("MAC адрес: ", WiFi.macAddress(), "\n");
  WRITE_INFO("SSID: ", WiFi.SSID(), "\n");
  WRITE_INFO("IP адрес: ", WiFi.localIP().toString().c_str(), "\n");
  WRITE_INFO("RSSI: ", WiFi.RSSI(), "\n");
}

//-------------------------------------------------------------------------------
static void onWiFiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  WRITE_INFO("Wi-Fi соединение потеряно.", "\n");
  WiFi.disconnect();
  wifi_init();
}
//-------------------------------------------------------------------------------

void wifi_init()
{
  if (WiFi.status() == WL_CONNECTED)
    return;
  if (millis() < next_connect_ms)
    return;

  if (f_init == true)
  {
    f_init = false;
    wifiConnectHandler = WiFi.onStationModeGotIP(onWiFiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWiFiDisconnect);
    WiFi.mode(WIFI_STA);
  }

  if (f_start_connect == false)
  {
    f_start_connect = true;
    next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
    WRITE_INFO("Попытка подключиться к WiFi...", "\n");
    WiFi.begin(ssid, password);
    return;
  }
  
  else
  {
    f_start_connect = false;
    WRITE_INFO("Wi-Fi соединение не установлено. Код ошибки: ", WiFi.status(), "\n");
    WRITE_INFO("Попытка снова через ", next_connect_ms / 1000, " секунд", "\n");
  }
}
//-------------------------------------------------------------------------------

void wifi_process()
{
  wifi_init();
}
//-------------------------------------------------------------------------------

bool get_wifi_connect_status()
{
  uint8_t status = WiFi.status();
  if (status == WL_CONNECTED)
  {
    return true;
  }
  return 0;
}
//-------------------------------------------------------------------------------