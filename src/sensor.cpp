#include "defines.h"
#include "mqtt.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

typedef struct
{
    float temperature;
    float humidity;
    volatile bool f_measure_ready;
    volatile unsigned long pause_ms;
    bool f_new_data;
    float delta;
} sensor_data_t;

static sensor_data_t sensor_data = {0, 0, false, 0, false, TEMPERAURE_DELTA};
static DHT dht(DHT_PIN, DHT_TYPE);
// static sensor_t sensor;
//-------------------------------------------------------------------------------
static void check_pause();
//-------------------------------------------------------------------------------

void sensor_init()
{
    WRITE_INFO("DHT22 стартует!", "\n");
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
        WRITE_INFO("Веду замеры...", "\n");
        mqttPrintf(LOG_TOPIC, "Веду замеры...");
        sensor_data.humidity = dht.readHumidity();
        sensor_data.temperature = dht.readTemperature()+sensor_data.delta;
        WRITE_INFO("delta = ", sensor_data.delta, "\n");
        if (isnan(sensor_data.humidity) || isnan(sensor_data.temperature))
        {
            WRITE_INFO("Неудача при чтении датчика.", "\n");
            mqttPrintf(LOG_TOPIC, "Неудача при чтении датчика.");
        }
        else
        {
            sensor_data.f_new_data = true;
            mqttPrintf(LOG_TOPIC, "Замеры успешны");
            WRITE_INFO("Замеры успешны: ");
            WRITE_INFO("Влажность: ", sensor_data.humidity, "%", "\n");
            WRITE_INFO("Температура: ", sensor_data.temperature, "°C ", "\n");
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

void set_temperature_delta(float d)
{
    sensor_data.delta = d;
}
//-------------------------------------------------------------------------------