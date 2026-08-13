#include <Arduino.h>
#include "defines.h"
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include "wifi.h"
#include "main_process.h"
#include "mqtt.h"
#include <ring_buf.h>
#include <timer.h>

static AsyncMqttClient client;
static Timer mqtt_reconnect_timer(MQTT_RECONNECT_TIMEOUT, true);
static bool f_try_connect = true;
// connect() запускает операцию, а результат придёт в callback.
// connect() starts the operation; its result arrives in a callback.
static bool f_connecting  = false;
// AsyncMqttClient может передать MQTT-сообщение несколькими фрагментами.
// AsyncMqttClient may deliver an MQTT message in several fragments.
static uint8_t incoming_payload[MQTT_QUEUE_PAYLOAD_SIZE];

typedef struct {
    char topic[MQTT_QUEUE_TOPIC_SIZE];
    char payload[MQTT_QUEUE_PAYLOAD_SIZE];
} mqtt_message_t;

static RingBuf mqtt_queue(sizeof(mqtt_message_t), MQTT_QUEUE_SIZE);

static void check_timeout_reconnect();
static void flush_mqtt_queue();
static void queue_mqtt_message(const char *topic, const char *payload);
static void on_mqtt_connect(bool session_present);
static void on_mqtt_disconnect(AsyncMqttClientDisconnectReason reason);
static void on_mqtt_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t length,
                            size_t index, size_t total);
//-------------------------------------------------------------------------------

static void mqttConnect()
{
    if (!get_wifi_connect_status() || !f_try_connect || f_connecting || client.connected())
        return;

    WRITE_INFO("Подключаюсь к MQTT...", "\n");
    client.connect();
    f_connecting  = true;
    f_try_connect = false;
}
//-------------------------------------------------------------------------------

void mqtt_process()
{
    check_timeout_reconnect();
    if (!client.connected())
        mqttConnect();
}
//-------------------------------------------------------------------------------

void mqtt_init()
{
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCredentials(MQTT_USER, MQTT_PASS);
    client.setClientId(MQTT_ID);
    client.onConnect(on_mqtt_connect);
    client.onDisconnect(on_mqtt_disconnect);
    client.onMessage(on_mqtt_message);
    mqttConnect();
}
//-------------------------------------------------------------------------------

static void check_timeout_reconnect()
{
    if (mqtt_reconnect_timer.elapsed())
        f_try_connect = true;
}
//-------------------------------------------------------------------------------

static void on_mqtt_connect(bool session_present)
{
    (void)session_present;
    f_connecting = false;
    WRITE_INFO("Подключение к MQTT успешно", "\n");
    client.subscribe(COMAND_TOPIC, 0);
    flush_mqtt_queue();
    mqttPrintf(MQTT_TOPIC_MY_IP, "%s", WiFi.localIP().toString().c_str());
    mqttPrintf(INFO_TOPIC, "%s, ВЕРСИЯ %s", DEVICE_NAME, VERSION);
}
//-------------------------------------------------------------------------------

static void on_mqtt_disconnect(AsyncMqttClientDisconnectReason reason)
{
    (void)reason;
    f_connecting = false;
    WRITE_INFO("Соединение с MQTT потеряно", "\n");
}
//-------------------------------------------------------------------------------

static void on_mqtt_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t length,
                            size_t index, size_t total)
{
    (void)properties;
    // Не копируем сообщение, которое не поместится в статический буфер.
    // Do not copy a message that does not fit in the static buffer.
    if (total > sizeof(incoming_payload) || index > total || length > total - index)
        return;

    memcpy(incoming_payload + index, payload, length);
    // Команду можно разбирать только после получения последнего фрагмента.
    // A command can be parsed only after the final fragment is received.
    if (index + length == total)
        parce_incoming_command(topic, incoming_payload, total);
}
//-------------------------------------------------------------------------------

// Пример: mqttPrintf("topic", "Привет, %s! Ответ: %d", "Мир", 42);
// Example: mqttPrintf("topic", "Hello, %s! The answer is %d", "World", 42);
void mqttPrintf(const char *topic, const char *format, ...)
{
    char buffer[MQTT_QUEUE_PAYLOAD_SIZE] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Ненулевой результат publish() означает приём библиотекой, а не подтверждённую доставку брокеру.
    // A nonzero publish() result means the library accepted the message, not confirmed broker delivery.
    if (!client.connected() || client.publish(topic, 0, false, buffer) == 0)
        queue_mqtt_message(topic, buffer);
}
//-------------------------------------------------------------------------------

static void queue_mqtt_message(const char *topic, const char *payload)
{
    mqtt_message_t message = {};

    strncpy(message.topic, topic, MQTT_QUEUE_TOPIC_SIZE - 1);
    strncpy(message.payload, payload, MQTT_QUEUE_PAYLOAD_SIZE - 1);
    mqtt_queue.push(&message);
}
//-------------------------------------------------------------------------------

static void flush_mqtt_queue()
{
    // publish() возвращает 0, если библиотека пока не приняла пакет в отправку.
    // publish() returns 0 when the library cannot accept the packet for sending yet.
    while (client.connected() && !mqtt_queue.empty()) {
        const mqtt_message_t *message = static_cast<const mqtt_message_t *>(mqtt_queue.front());
        if (client.publish(message->topic, 0, false, message->payload) == 0)
            return;

        mqtt_queue.discard();
    }
}
//-------------------------------------------------------------------------------
