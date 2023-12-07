#include "defines.h"
//#include "DHTesp.h"
#include "mqtt.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
//#include <DHT_U.h>

typedef struct
{
    float temperature;
    float humidity;
    volatile bool f_measure_ready;
    volatile unsigned long pause_ms;
    bool f_new_data;
} sensor_data_t;

static sensor_data_t sensor_data = {0, 0, false, 0, false};
//static DHTesp dht;
static DHT dht(DHT_PIN, DHT_TYPE);
static sensor_t sensor;
//-------------------------------------------------------------------------------
static void check_pause();
//-------------------------------------------------------------------------------

void sensor_init()
{
    Serial.println(F("DHT22 стартует!"));
    dht.begin();
    sensor_data.pause_ms = millis() + DHT_STABLE_MS;
}
//-------------------------------------------------------------------------------

void process_sensor()
{
    check_pause();
    if (sensor_data.f_measure_ready == true)
    {
        sensor_data.f_measure_ready = false;
        mqttPrintf(LOG_TOPIC, "Веду замеры...");
        Serial.println(F("Веду замеры..."));
        sensor_data.humidity = dht.readHumidity();
        sensor_data.temperature = dht.readTemperature();
        if (isnan(sensor_data.humidity) || isnan(sensor_data.temperature))
        {
            mqttPrintf(LOG_TOPIC, "Неудача при чтении датчика.");
            Serial.println(F("Неудача при чтении датчика."));
        }
        else
        {
            sensor_data.f_new_data = true;
            mqttPrintf(LOG_TOPIC, "Замеры успешны");
            Serial.println(F("Замеры успешны: "));
            Serial.print(F("Влажность: "));
            Serial.print(sensor_data.humidity);
            Serial.print(F("%  Температура: "));
            Serial.print(sensor_data.temperature);
            Serial.println(F("°C "));
        }
    }
}
//-------------------------------------------------------------------------------

bool get_new_data()
{
    if (sensor_data.f_new_data == true)
    {
        sensor_data.f_new_data = false;
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------

static void check_pause()
{
    if (millis() > sensor_data.pause_ms)
    {
        sensor_data.f_measure_ready = true;
        sensor_data.pause_ms = millis() + DHT_READ_PERIOD;
    }
}
//-------------------------------------------------------------------------------

float get_temperature()
{
    return sensor_data.temperature;
}
//-------------------------------------------------------------------------------

float get_humidity()
{
    return sensor_data.humidity;
}
//-------------------------------------------------------------------------------
