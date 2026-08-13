#include "main_process.h"

#include "defines.h"
#include "sensor.h"
#include "mqtt.h"
#include "gpio.h"
#include <ESP8266WiFi.h>
#include <timer.h>

struct uptime_t {
    uint32_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

static int minSpeed                      = FAN_SPEED_MIN;
static int maxSpeed                      = FAN_SPEED_MAX;
static float tempCoefficient             = TEMPERATURE_COEFFICIENT;
static float humidityCoefficient         = HUMIDITY_COEFFICIENT;
static volatile int fanSpeed             = 0;
static volatile int fan_pwm              = 0;
static bool f_auto_mode                  = true;
static Timer info_timer(1000);
static Timer fan_speed_timer(1000);
static uptime_t uptime = {};
static PulseCounter fan_tachometer(FAN_SPEED_PIN, FALLING, INPUT_PULLUP);
static DebouncedInput button(BUTTON_PIN, INPUT_PULLUP);
static bool gpio_output_state = false;
//-------------------------------------------------------------------------------
static void relay_toggle(bool state);
static void info_process();
//-------------------------------------------------------------------------------

void main_init()
{
    Serial.begin(SERIAL_BAUDRATE);
    WRITE_INFO("\n", "\n", "\n", "-----------------------------------\n");
    WRITE_INFO("Имя: ", DEVICE_NAME, "\n");
    WRITE_INFO("Версия: ", VERSION, "\n");
    WRITE_INFO("Стартую!\n");

    pinMode(RELAY_PIN, OUTPUT);
    // Тахометр вентилятора обычно имеет выход с открытым коллектором.
    // The fan tachometer usually has an open-collector output.
    fan_tachometer.begin();
    button.begin();
    pinMode(INDICATOR_PIN, OUTPUT);
    digitalWrite(INDICATOR_PIN, gpio_output_state);
}
//-------------------------------------------------------------------------------

void main_process()
{
    if (fan_speed_timer.elapsed()) {
        fanSpeed = fan_tachometer.readAndReset();
    }

    if (button.update() && !button.read()) {
        gpio_output_state = !gpio_output_state;
        digitalWrite(INDICATOR_PIN, gpio_output_state);
    }

    if (get_new_data()) {
        if (f_auto_mode) {
            fan_pwm = minSpeed + (get_temperature() - TARGET_TEMPERATURE) * tempCoefficient +
                      (get_humidity() - TARGET_HUMIDITY) * humidityCoefficient;

            if (fan_pwm <= minSpeed) {
                fan_pwm = minSpeed;
                relay_toggle(OFF);
            } else if (fan_pwm > maxSpeed) {
                fan_pwm = maxSpeed;
                relay_toggle(ON);
            } else
                relay_toggle(ON);
        }

        analogWrite(FAN_PIN, fan_pwm);
        WRITE_INFO("Скважность ШИМ: ", fan_pwm, "\n");
        WRITE_INFO("Обороты вентилятора: ", fanSpeed, "\n");
        WRITE_INFO("Состояние реле: ", digitalRead(RELAY_PIN), "\n");
        if (f_auto_mode) {
            WRITE_INFO("Режим работы: автономный", "\n");
            mqttPrintf(FAN_SPEED_TOPIC, "%d", fanSpeed);
        } else
            WRITE_INFO("Режим работы: ручной", "\n");

        mqttPrintf(TEMPERATURE_TOPIC, "%.1f", get_temperature());
        mqttPrintf(HUMIDITY_TOPIC, "%.1f", get_humidity());
        mqttPrintf(RELAY_TOPIC, "%d", digitalRead(RELAY_PIN));
        mqttPrintf(FAN_PWM_TOPIC, "%d", fan_pwm);
    }
    info_process();
}
//-------------------------------------------------------------------------------

static void relay_toggle(bool state)
{
    if (digitalRead(RELAY_PIN) == state)
        return;

    digitalWrite(RELAY_PIN, state);
    mqttPrintf(RELAY_TOPIC, "%d", digitalRead(RELAY_PIN));
    WRITE_INFO("Реле переключено, состояние: ", digitalRead(RELAY_PIN), "\n");
}
//-------------------------------------------------------------------------------

// Используется как callback входящего MQTT-сообщения.
// Used as the callback for an incoming MQTT message.
void parce_incoming_command(char *topic, byte *payload, unsigned int length)
{
    (void)topic;
    char command[length + 1];
    for (uint32_t i = 0; i < length; i++)
        command[i] = (char)payload[i];
    command[length] = '\0';

    if (strcmp(command, COMAND_RESTART) == 0)
        ESP.restart();
    else if (strcmp(command, COMAND_RELAY_ON) == 0)
        relay_toggle(true);
    else if (strcmp(command, COMAND_RELAY_OFF) == 0)
        relay_toggle(false);
    else if (strcmp(command, COMAND_SET_MODE_AUTO) == 0)
        f_auto_mode = true;
    else if (strcmp(command, COMAND_SET_MODE_MANUAL) == 0)
        f_auto_mode = false;
    else if (strncmp(command, COMAND_SET_PWM, sizeof(COMAND_SET_PWM) - 1) == 0) {
        // С индекса 4 идёт число от 0 до 255 для вентилятора.
        // From index 4, a number from 0 to 255 specifies the fan value.
        fan_pwm = atoi(&command[sizeof(COMAND_SET_PWM) - 1]);
        WRITE_INFO(COMAND_SET_PWM, ": ", fan_pwm, "\n");
        analogWrite(FAN_PIN, fan_pwm);
    } else if (strncmp(command, COMAND_SET_TEMPERATURE_DELTA, sizeof(COMAND_SET_TEMPERATURE_DELTA) - 1) == 0) {
        float d = atof(&command[sizeof(COMAND_SET_TEMPERATURE_DELTA) - 1]);
        WRITE_INFO(COMAND_SET_TEMPERATURE_DELTA, " = ", d, "\n");
        set_temperature_delta(d);
    }
    mqttPrintf(LOG_TOPIC, "%s", command);
    WRITE_INFO(String("Команда: ") + command + "\n");
}
//-------------------------------------------------------------------------------

static void info_process()
{
    if (info_timer.elapsed()) {
        mqttPrintf(INFO_TOPIC, "%s, ВЕРСИЯ %s", DEVICE_NAME, VERSION);

        char ip_str[16];
        snprintf(ip_str, sizeof(ip_str), "%s", WiFi.localIP().toString().c_str());
        mqttPrintf(MQTT_TOPIC_MY_IP, "%s", ip_str);
        mqttPrintf(WIFI_SIGNAL_TOPIC, "%d", WiFi.RSSI());

        uint32_t uptime_ms = millis();
        uptime.days        = uptime_ms / 86400000;
        uptime_ms          = uptime_ms % 86400000;
        uptime.hours       = uptime_ms / 3600000;
        uptime_ms          = uptime_ms % 3600000;
        uptime.minutes     = uptime_ms / 60000;
        uptime_ms          = uptime_ms % 60000;
        uptime.seconds     = uptime_ms / 1000;

        mqttPrintf(MQTT_TOPIC_UPTIME, "%dд:%dч:%dм:%dс", uptime.days, uptime.hours, uptime.minutes, uptime.seconds);
        WRITE_INFO("Время работы: ", uptime.days, ":", uptime.hours, ":", uptime.minutes, ":", uptime.seconds, "\n");

        info_timer.setInterval(MQTT_INFO_MS);
    }
}
//-------------------------------------------------------------------------------
