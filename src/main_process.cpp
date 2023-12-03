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
    Serial.println();
    Serial.println("Стартую!");
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
                fan_pwm = maxSpeed;
            else
                relay_toggle(ON);
        }

        analogWrite(FAN_PIN, fan_pwm);
        Serial.print("ШИМ вентилятора: ");
        Serial.println(fan_pwm);
        Serial.print("Обороты вентилятора: ");
        Serial.println(fanSpeed);
        Serial.print("Состояние реле: ");
        Serial.println(digitalRead(RELAY_PIN));
        if(f_auto_mode){
            Serial.println("Режим работы: автономный");
            mqttPrintf(FAN_SPEED_TOPIC, "%d", fanSpeed);
            }
        else{
            Serial.print("Режим работы: ручной");}
        mqttPrintf(FAN_SPEED_TOPIC, "%d", fanSpeed);
        mqttPrintf(TEMPERATURE_TOPIC, "%.1f", get_temperature());
        mqttPrintf(HUMIDITY_TOPIC, "%.1f", get_humidity());
        mqttPrintf(RELAY_TOPIC, "%d", digitalRead(RELAY_PIN));
        mqttPrintf(FAN_SPEED_TOPIC, "%d", fanSpeed);
    }
}
//-------------------------------------------------------------------------------

static void relay_toggle(bool state)
{
    if(digitalRead(RELAY_PIN)==state) 
        return;
    
    digitalWrite(RELAY_PIN, state);
    mqttPrintf(RELAY_TOPIC, "%d", digitalRead(RELAY_PIN));
    Serial.print("Реле переключено, состояние: ");
    Serial.println(digitalRead(RELAY_PIN));
}
//-------------------------------------------------------------------------------

void parce_incoming_command(char *topic, byte *payload, unsigned int length)
{//используется в качестве коллбека при входящемм сообщении MQTT
    char command[length + 1];
    for (uint32_t i = 0; i < length; i++)
        command[i] = (char)payload[i];

    command[length] = '\0';

    if (strcmp(command, "on") == 0)
        relay_toggle(true);
    else if (strcmp(command, "off") == 0)
        relay_toggle(false);
    else if (strcmp(command, "auto") == 0)
        f_auto_mode = true;
    else if (strcmp(command, "manual") == 0)
        f_auto_mode = false;
    else if (strcmp(command, "PWM ") == 0)
    {
        // начиная с индекса 4 - идет число(0-255) для вентилятора
        fan_pwm = atoi(&command[4]);
        analogWrite(FAN_PIN, fan_pwm);
    }
    mqttPrintf(LOG_TOPIC, "%s", command);
}

//-------------------------------------------------------------------------------
