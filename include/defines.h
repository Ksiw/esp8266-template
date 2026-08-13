#pragma once

// Общие настройки.
// Common settings.
#define SERIAL_INFO                  true
#define VERSION                      "0.2.0"
#define DEVICE_NAME                  "TEMPLATE"
#define SERIAL_BAUDRATE              115200
#define TARGET_TEMPERATURE           29
#define TARGET_HUMIDITY              50
#define ON                           1
#define OFF                          0
#define DEVICE_PREFIX                "p58/template/"
#define LOCAL_SERVER_IP              "192.168.0.17"

// Wi-Fi.
#define WIFI_SSID                    "" // <------------------<<<
#define WIFI_PASSWORD                "" // <------------------<<<
#define WIFI_CONNECT_TIMEOUT         20000

// MQTT.
#define MQTT_INFO_MS                 20000UL
#define MQTT_SERVER                  LOCAL_SERVER_IP
#define MQTT_PORT                    1883
// Идентификатор MQTT-клиента.
// MQTT client identifier.
#define MQTT_ID                      "mqtt_21223"
#define MQTT_USER                    "template"
#define MQTT_PASS                    "pass"
#define MQTT_RECONNECT_TIMEOUT       10000
#define MQTT_QUEUE_SIZE              10
#define MQTT_QUEUE_TOPIC_SIZE        96
#define MQTT_QUEUE_PAYLOAD_SIZE      128
#define DEVICE_PREFIX_DATA           DEVICE_PREFIX "data/"
#define DEVICE_PREFIX_INFO           DEVICE_PREFIX "info/"
#define COMAND_TOPIC                 DEVICE_PREFIX "comand"
#define MQTT_TOPIC_MY_IP             DEVICE_PREFIX_INFO "my_ip"
#define INFO_TOPIC                   DEVICE_PREFIX_INFO "info"
#define LOG_TOPIC                    DEVICE_PREFIX_INFO "log"
#define TIME_TOPIC                   DEVICE_PREFIX_DATA "time"
#define NOISE_TOPIC                  DEVICE_PREFIX_DATA "noise"
#define HUMIDITY_TOPIC               DEVICE_PREFIX_DATA "humidity"
#define TEMPERATURE_TOPIC            DEVICE_PREFIX_DATA "temperature"
#define FAN_SPEED_TOPIC              DEVICE_PREFIX_DATA "fan_speed"
#define RELAY_TOPIC                  DEVICE_PREFIX_DATA "relay"
#define FAN_PWM_TOPIC                DEVICE_PREFIX_DATA "fan_pwm"
#define WIFI_SIGNAL_TOPIC            DEVICE_PREFIX_DATA "wifi_signal"
#define MQTT_TOPIC_UPTIME            DEVICE_PREFIX_INFO "uptime"

// Датчик.
// Sensor.
#define DHT_PIN                      D4
#define DHT_TYPE                     DHT22
#define DHT_STABLE_MS                2000
#define DHT_READ_PERIOD              15000
#define TEMPERAURE_DELTA             -2.5f

// Нагрузка.
// Load.
// На плате D1 пин реле нужно перепаять.
// The relay pin must be rewired on the D1 board.
#define RELAY_PIN                    D5
#define FAN_PIN                      D6
#define FAN_SPEED_MAX                255
#define FAN_SPEED_MIN                0
#define TEMPERATURE_COEFFICIENT      1.1
#define HUMIDITY_COEFFICIENT         1.7
#define FAN_SPEED_PIN                D7

// GPIO.

// Кнопка подключается между BUTTON_PIN и GND; используется INPUT_PULLUP.
// Connect the button between BUTTON_PIN and GND; INPUT_PULLUP is used.
#define BUTTON_PIN                   D3
#define INDICATOR_PIN                D4

// Дисплей.
// Display.
#define LCD_WIDTH                    64
#define LCD_HEIGHT                   48
#define LCD_TEMPERATURE_MS           1500
#define LCD_HUMIDITY_MS              1500
#define LCD_NOISE_MS                 2000
#define OLED_RESET                   -1
#define SCREEN_ADDRESS               0x3C

// NTP.
// NTP.
#define NTP_SERVER                   LOCAL_SERVER_IP
// Часовой пояс Москвы.
// Moscow time zone.
#define NPT_TIME_ZONE                3
#define NPT_UPDATE_PERIOD_MS         60 * 60 * 1000
// Интервал следующей попытки после неудачного обновления.
// Retry interval after a failed update.
#define NPT_MISTAKE_UPDATE_PERIOD_MS 20000
#define NTP_DEFAULT_LOCAL_PORT       123
#define NTP_WAIT_UPDATE_MS           1000

// Сторожевой таймер.
// Watchdog timer.
#define WATCHDOG_TIMEOUT             10000

// OTA-обновление.
// OTA update.
#define OTA_USER                     ""
#define OTA_PASSWORD                 ""
#define OTA_PORT                     80
#define OTA_PATH                     "/"

// Команды.
// Commands.
#define COMAND_RESTART               "restart"
#define COMAND_RELAY_ON              "on"
#define COMAND_RELAY_OFF             "off"
#define COMAND_SET_MODE_AUTO         "auto"
#define COMAND_SET_MODE_MANUAL       "manual"
#define COMAND_SET_PWM               "PWM"
#define COMAND_SET_TEMPERATURE_DELTA "delta"

// Макросы.
// Macros.
// Пример: WRITE_INFO("Версия: ", VERSION, "\n");
// Example: WRITE_INFO("Version: ", VERSION, "\n");
#if SERIAL_INFO
#include <Arduino.h>
template <typename... Args>
void WRITE_INFO(const Args &...args)
{
    (Serial.print(args), ...);
}
#else
#define WRITE_INFO(...)
#endif
