#include "main_process.h"

#include "defines.h"
#include "sensor.h"
#include "mqtt.h"

static int minSpeed = FAN_SPEED_MIN;
static int maxSpeed = FAN_SPEED_MAX;
static float tempCoefficient = TEMPERATURE_COEFFICIENT;
static float humidityCoefficient = HUMIDITY_COEFFICIENT;
static volatile unsigned long pulseCount = 0;
static volatile int fanSpeed = 0;
static volatile int fan_pwm = 0;
static bool f_auto_mode = true;
//-------------------------------------------------------------------------------
static void relay_toggle(bool state);
//-------------------------------------------------------------------------------

static void IRAM_ATTR countPulses()
{
    pulseCount++; // счетчик импульсов при каждом прерывании
}
//-------------------------------------------------------------------------------

static void calculateSpeed()
{
    fanSpeed = pulseCount;
    pulseCount = 0;
}
//-------------------------------------------------------------------------------

void main_init()
{
    Serial.begin(SERIAL_BAUDRATE);
    WRITE_INFO("\n", "\n", "\n", "-----------------------------------\n");
    WRITE_INFO("Имя: ", DEVICE_NAME, "\n");
    WRITE_INFO("Версия: ", VERSION, "\n");
    WRITE_INFO("Стартую!\n");

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(FAN_SPEED_PIN, INPUT_PULLDOWN_16);
    attachInterrupt(digitalPinToInterrupt(FAN_SPEED_PIN), countPulses, RISING); // прерывание на изменение состояния пина D7 на HIGH
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);                              // настройка таймера 1 с предделителем 256 и режимом прерывания на каждый фронт
    timer1_attachInterrupt(calculateSpeed);
    timer1_write(1000000); // установка периода таймера на 1 секунду
}
//-------------------------------------------------------------------------------

void main_process()
{
    if (get_new_data())
    {
        if (f_auto_mode)
        {
            fan_pwm = minSpeed + (get_temperature() - TARGET_TEMPERATURE) * tempCoefficient +
                      (get_humidity() - TARGET_HUMIDITY) * humidityCoefficient;

            if (fan_pwm <= minSpeed)
            {
                fan_pwm = minSpeed;
                relay_toggle(OFF);
            }
            else if (fan_pwm > maxSpeed)
            {
                fan_pwm = maxSpeed;
                relay_toggle(ON);
            }
            else
                relay_toggle(ON);
        }

        analogWrite(FAN_PIN, fan_pwm);
        WRITE_INFO("Скважность ШИМ: ", fan_pwm, "\n");
        WRITE_INFO("Обороты вентилятора: ", fanSpeed, "\n");
        WRITE_INFO("Состояние реле: ", digitalRead(RELAY_PIN), "\n");
        if (f_auto_mode)
        {
            WRITE_INFO("Режим работы: автономный", "\n");
            mqttPrintf(FAN_SPEED_TOPIC, "%d", fanSpeed);
        }
        else
            WRITE_INFO("Режим работы: ручной", "\n");

        mqttPrintf(FAN_SPEED_TOPIC, "%d", fanSpeed);
        mqttPrintf(TEMPERATURE_TOPIC, "%.1f", get_temperature());
        mqttPrintf(HUMIDITY_TOPIC, "%.1f", get_humidity());
        mqttPrintf(RELAY_TOPIC, "%d", digitalRead(RELAY_PIN));
        mqttPrintf(FAN_PWM_TOPIC, "%d", fan_pwm);
    }
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

void parce_incoming_command(char *topic, byte *payload, unsigned int length)
{ // используется в качестве коллбека при входящем сообщении MQTT
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
    else if (strncmp(command, COMAND_SET_PWM, sizeof(COMAND_SET_PWM) - 1) == 0)
    {
        // начиная с индекса 4 - идет число(0-255) для вентилятора
        fan_pwm = atoi(&command[sizeof(COMAND_SET_PWM) - 1]);
        WRITE_INFO(COMAND_SET_PWM, ": ", fan_pwm, "\n");
        analogWrite(FAN_PIN, fan_pwm);
    }
    else if (strncmp(command, COMAND_SET_TEMPERATURE_DELTA, sizeof(COMAND_SET_TEMPERATURE_DELTA) - 1) == 0)
    {
        float d = atof(&command[sizeof(COMAND_SET_TEMPERATURE_DELTA) - 1]);
        WRITE_INFO(COMAND_SET_TEMPERATURE_DELTA, " = ", d, "\n");
        set_temperature_delta(d);
    }
    mqttPrintf(LOG_TOPIC, "%s", command);
    WRITE_INFO(String("Команда: ") + command + "\n");
}

//-------------------------------------------------------------------------------
