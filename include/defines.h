#pragma once

//wifi
#define WIFI_SSID               "------"
#define WIFI_PASSWORD           "------"
#define WIFI_CONNECT_TIMEOUT    10000

//mqtt
#define MQTT_SERVER             "192.168.0.200"
#define MQTT_PORT               1883
#define MQTT_ID                 "mqtt_21223" //дата
#define MQTT_USER               "---"
#define MQTT_PASS               "---"
#define MQTT_RECONNECT_TIMEOUT  10000

//Common
#define SERIAL_BAUDRATE         921600
#define TARGET_TEMPERATURE      24
#define TARGET_HUMIDITY         50
#define ON                      1
#define OFF                     0

//sensor
#define DHT_PIN                 D4
#define DHT_TYPE                DHT22
#define DHT_READ_PERIOD         15000
#define LOG_MESS_SIZE           64
#define LOG_TOPIC               "p58/kitch_ventilation/log"
#define HUMIDITY_TOPIC          "p58/kitch_ventilation/humidity"
#define TEMPERATURE_TOPIC       "p58/kitch_ventilation/temperature"
#define FAN_SPEED_TOPIC         "p58/kitch_ventilation/fan_speed"
#define RELAY_TOPIC             "p58/kitch_ventilation/relay"
#define TOPIC_COMAND            "p58/kitch_ventilation/"

//load
#define RELAY_PIN               D5
#define FAN_PIN                 D6
#define FAN_SPEED_MAX           255
#define FAN_SPEED_MIN           0
#define TEMPERATURE_COEFFICIENT 1.3
#define HUMIDITY_COEFFICIENT    1.7
#define FAN_SPEED_PIN           D0

//lcd
#define LCD_WIDTH               64
#define LCD_HEIGHT              48
#define LCD_TEMPERATURE_MS      2000
#define LCD_HUMIDITY_MS         2000
#define LCD_NOISE_MS            2000
#define OLED_RESET              -1
#define SCREEN_ADDRESS          0x3C
