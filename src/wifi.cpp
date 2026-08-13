#include <ESP8266WiFi.h>
#include "defines.h"
#include "wifi.h"
#include <timer.h>

#define CHECK_CONNECT_MS 100

static const char *ssid     = WIFI_SSID;
static const char *password = WIFI_PASSWORD;

static WiFiEventHandler wifiConnectHandler;
static WiFiEventHandler wifiDisconnectHandler;

static Timer connect_timer(WIFI_CONNECT_TIMEOUT, true);
static bool f_init          = true;
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
    if (f_start_connect && !connect_timer.elapsed())
        return;

    if (f_init == true) {
        f_init                = false;
        // Обработчики регистрируются один раз: Wi-Fi вызывает их из своего event loop.
        // Handlers are registered once: Wi-Fi calls them from its event loop.
        wifiConnectHandler    = WiFi.onStationModeGotIP(onWiFiConnect);
        wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWiFiDisconnect);
        WiFi.mode(WIFI_STA);
    }

    if (f_start_connect == false) {
        f_start_connect = true;
        connect_timer.reset();
        WRITE_INFO("Попытка подключиться к WiFi...", "\n");
        WiFi.begin(ssid, password);
        return;
    }

    else {
        f_start_connect = false;
        WRITE_INFO("Wi-Fi соединение не установлено. Код ошибки: ", WiFi.status(), "\n");
        WRITE_INFO("Повторная попытка подключения к WiFi...", "\n");
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
    if (status == WL_CONNECTED) {
        return true;
    }
    return 0;
}
//-------------------------------------------------------------------------------
