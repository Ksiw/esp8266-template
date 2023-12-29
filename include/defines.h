#pragma once

//Common
#define SERIAL_INFO                     true
#define VERSION                         "0.1.0"
#define DEVICE_NAME                     "TEMPLATE"
#define SERIAL_BAUDRATE                 115200
#define TARGET_TEMPERATURE              29
#define TARGET_HUMIDITY                 50
#define ON                              1
#define OFF                             0
#define DEVICE_PREFIX                   "p58/hallway_fan/"
#define DEVICE_PREFIX_DATA              DEVICE_PREFIX"data/"
#define LOCAL_SERVER_IP                 "192.168.0.17"
#define MDNS_NAME                       "TEMPLATE"
#define UPDATE_USERNAME                 "user"
#define UPDATE_PASS                     "pass"

//wifi
#define WIFI_SSID                       "SSID"
#define WIFI_PASSWORD                   "pass"
#define WIFI_CONNECT_TIMEOUT            15000

//mqtt
#define MQTT_SERVER                     LOCAL_SERVER_IP
#define MQTT_PORT                       1883
#define LOG_MESS_SIZE                   64
#define MQTT_ID                         "mqtt_21223" //дата
#define MQTT_USER                       "user"
#define MQTT_PASS                       "pass"
#define MQTT_TOPIC_MY_IP                DEVICE_PREFIX"my_ip"
#define MQTT_RECONNECT_TIMEOUT          10000
#define MAX_QUEUE_SIZE                  20

//sensor
#define DHT_PIN                         D4
#define DHT_TYPE                        DHT22
#define DHT_STABLE_MS                   2000
#define DHT_READ_PERIOD                 15000
#define TEMPERAURE_DELTA                -2.5f
#define INFO_TOPIC                      DEVICE_PREFIX"info"
#define LOG_TOPIC                       DEVICE_PREFIX"log"
#define COMAND_TOPIC                    DEVICE_PREFIX"comand"
#define TIME_TOPIC                      DEVICE_PREFIX_DATA"time"
#define NOISE_TOPIC                     DEVICE_PREFIX_DATA"noise"
#define HUMIDITY_TOPIC                  DEVICE_PREFIX_DATA"humidity"
#define TEMPERATURE_TOPIC               DEVICE_PREFIX_DATA"temperature"
#define FAN_SPEED_TOPIC                 DEVICE_PREFIX_DATA"fan_speed"
#define RELAY_TOPIC                     DEVICE_PREFIX_DATA"relay"
#define FAN_PWM_TOPIC                   DEVICE_PREFIX_DATA"fan_pwm"

//load
#define RELAY_PIN                       D5 //на плате D1, перепаяй!
#define FAN_PIN                         D6
#define FAN_SPEED_MAX                   255
#define FAN_SPEED_MIN                   0
#define TEMPERATURE_COEFFICIENT         1.1
#define HUMIDITY_COEFFICIENT            1.7
#define FAN_SPEED_PIN                   D0

//lcd
#define LCD_WIDTH                       64
#define LCD_HEIGHT                      48
#define LCD_TEMPERATURE_MS              1500
#define LCD_HUMIDITY_MS                 1500
#define LCD_NOISE_MS                    2000
#define OLED_RESET                      -1
#define SCREEN_ADDRESS                  0x3C

//ntp
#define NTP_SERVER                      LOCAL_SERVER_IP
#define NPT_TIME_ZONE                   3                //часовой пояс Москвы
#define NPT_UPDATE_PERIOD_MS            60*60*1000
#define NPT_MISTAKE_UPDATE_PERIOD_MS    20000            //следующее обновление, если предыдущее неудачно 
#define NTP_DEFAULT_LOCAL_PORT          123
#define NTP_WAIT_UPDATE_MS              1000

//watchdog
#define WATCHDOG_TIMEOUT                10000

//ota
#define OTA_USER                        "user"
#define OTA_PASSWORD                    "pass"
#define OTA_PORT                        80
#define OTA_PATH                        "/update"

//comands
#define COMAND_RELAY_ON                 "on"
#define COMAND_RELAY_OFF                "off"
#define COMAND_SET_MODE_AUTO            "auto"
#define COMAND_SET_MODE_MANUAL          "manual"
#define COMAND_SET_PWM                  "PWM"
#define COMAND_SET_TEMPERATURE_DELTA    "delta"

 

//macros
//example    WRITE_INFO("Версия: ", VERSION, "\n");
#if SERIAL_INFO 
//#if (SERIAL_INFO == true)
#include <Arduino.h>
template<typename... Args>
void WRITE_INFO(const Args&... args) {
    (Serial.print(args), ...);
}
#else
#define WRITE_INFO(...)
#endif


