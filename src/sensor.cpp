#include "defines.h"
#include "DHTesp.h"
#include "mqtt.h"

typedef struct
{
    float temperature;
    float humidity;
    volatile bool f_measure_ready;
    volatile unsigned long pause_ms;
    bool f_new_data;
} sensor_data_t;

sensor_data_t sensor_data = {0, 0, false, 0, false};
DHTesp dht;
//-------------------------------------------------------------------------------
void check_pause();
//-------------------------------------------------------------------------------

void sensor_init()
{
    Serial.println(F("Датчик стартует..."));
    dht.setup(DHT_PIN, DHTesp::DHT_TYPE);
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
        sensor_data.humidity = dht.getHumidity();
        sensor_data.temperature = dht.getTemperature();
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
            Serial.print(F("Humidity: "));
            Serial.print(sensor_data.humidity);
            Serial.print(F("%  Temperature: "));
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

void check_pause()
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
