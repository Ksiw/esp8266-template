/*
  Шаблон домашней автоматики для ESP8266, разработанный в PlatformIO.
  Управляет нагрузкой по температуре и влажности, пишет лог в Serial Monitor и MQTT.
  Настройки вынесены в отдельный файл defines.h.
  SSD1306 поочерёдно показывает температуру и влажность; между экранами — мерцающие точки.
*/

/*
  ESP8266 home-automation template developed with PlatformIO.
  Controls a load by temperature and humidity, and logs to Serial Monitor and MQTT.
  Settings are kept in the separate defines.h file.
  SSD1306 alternates temperature and humidity screens, with flickering dots in between.
*/

#include <Arduino.h>
#include "defines.h"
#include "sensor.h"
#include "display.h"
#include "wifi.h"
#include "mqtt.h"
#include "ntp.h"
#include "main_process.h"
#include "gpio.h"
#include "firmware_update.h"
//-------------------------------------------------------------------------------

void setup()
{
#ifndef DEBUG_MODE
    wdt_enable(WATCHDOG_TIMEOUT);
#endif
    gpio_module_init();
    main_init();
    // Инициализируйте датчик, если он используется в устройстве.
    // Initialize the sensor when the device uses it.
    // sensor_init();
    // Инициализируйте дисплей, если он используется в устройстве.
    // Initialize the display when the device uses it.
    // display_init();
    wifi_init();
    ntp_init();
    mqtt_init();
    firmware_update_init();
}
//-------------------------------------------------------------------------------

void loop()
{
    wdt_reset();
    gpio_module_process();
    // Обрабатывайте датчик, если он используется в устройстве.
    // Process the sensor when the device uses it.
    // process_sensor();
    // Обрабатывайте дисплей, если он используется в устройстве.
    // Process the display when the device uses it.
    // display_process();
    wifi_process();
    mqtt_process();
    main_process();
    ntp_process();
    firmware_update_process();
}
//-------------------------------------------------------------------------------
