/*
  Данный проект являет собой шаблон для домашней автоматики на ESP8266. Разработан в среде PlatformIO.
  Предназначен для управления нагрузкой(вентилятором) в зависимости от показаний датчика температуры и влажности.
  Ведет лог в Serial Monitor и MQTT.
  Для удобства настройки дефайны вынесены в отдельный файл.
  Имеется вывод температуры и влажности на дисплей ssd1306. Данный дисплей слонен к выгоранию,
  поэтому между экранами с полезным выводом имеется анимация в вижде мерцающих точек.
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
  gpio_init();
  main_init();
  sensor_init();
  display_init();
  wifi_init();
  ntp_init();
  mqtt_init();
  firmware_update_init();

}
//-------------------------------------------------------------------------------

void loop()
{
  wdt_reset();
  gpio_process();
  process_sensor();
  display_process();
  wifi_process();
  mqtt_process();
  main_process();
  ntp_process();
  firmware_update_process();
}
//-------------------------------------------------------------------------------
