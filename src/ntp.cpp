#include <Arduino.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "defines.h"
#include "wifi.h"
#include "mqtt.h"
#include "ntp.h"
#include <timer.h>

typedef enum {
    idle,
    ready_to_start,
    starting,
} ntp_state_t;

#define NTP_PACKET_SIZE 48
#define NTP_LOCAL_PORT  2390

static Timer update_timer(0, true);
static WiFiUDP udp;
static uint8_t packetBuffer[NTP_PACKET_SIZE] = {0};
static IPAddress ntp_server_ip;

static void ntp_time_process();
static bool sendNTPpacket(const IPAddress &address, uint16_t port);
String getFormattedTime();
static bool f_init_done      = false;
static ntp_state_t ntp_state = idle;
static Timer wait_timer(NTP_WAIT_UPDATE_MS);
//-------------------------------------------------------------------------------

void ntp_init()
{
    if (f_init_done == true)
        return;
    f_init_done = true;
    WRITE_INFO("Инициализация NTP...", "\n");
    udp.begin(NTP_LOCAL_PORT);
    ntp_server_ip.fromString(NTP_SERVER);
}
//-------------------------------------------------------------------------------

void ntp_process()
{
    if (get_wifi_connect_status()) {
        if (ntp_state == idle && update_timer.elapsed()) {
            WRITE_INFO("Попытка обновления времени...", "\n");
            mqttPrintf(LOG_TOPIC, "Попытка обновления времени");
            ntp_state = ready_to_start;
        }

        ntp_time_process();
    }
}
//-------------------------------------------------------------------------------

static void ntp_time_process()
{
    // Один вызов делает один шаг автомата и никогда не ждёт ответ блокирующе.
    // One call advances the state machine once and never blocks waiting for a reply.
    switch (ntp_state) {
    case idle:
        return;

    case ready_to_start:
        WRITE_INFO("Попытка запроса NTP...", "\n");
        if (!sendNTPpacket(ntp_server_ip, NTP_DEFAULT_LOCAL_PORT)) {
            WRITE_INFO("Не удалось отправить NTP-запрос\n");
            update_timer.setInterval(NPT_MISTAKE_UPDATE_PERIOD_MS);
            update_timer.reset();
            ntp_state = idle;
            break;
        }
        wait_timer.reset();
        ntp_state = starting;
        break;

    case starting:
        if (udp.parsePacket() > 0) {
            int size = udp.read(packetBuffer, NTP_PACKET_SIZE);
            if (size == NTP_PACKET_SIZE) {
                WRITE_INFO("Принят NTP ответ", "\n");
                // В NTP timestamp начинается с байта 40 и хранится в big-endian.
                // The NTP timestamp starts at byte 40 and is stored big-endian.
                unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
                unsigned long lowWord  = word(packetBuffer[42], packetBuffer[43]);
                time_t secsSince1900   = highWord << 16 | lowWord;
                secsSince1900          = secsSince1900 - 2208988800UL + NPT_TIME_ZONE * SECS_PER_HOUR;
                setTime(secsSince1900);
                update_timer.setInterval(NPT_UPDATE_PERIOD_MS);
                update_timer.reset();
                mqttPrintf(TIME_TOPIC, getFormattedTime().c_str());
                WRITE_INFO("Время: ", getFormattedTime(), "\n");
                ntp_state = idle;
                break;
            }
        }

        if (wait_timer.elapsed()) {
            WRITE_INFO("NTP не отвечает", "\n");
            mqttPrintf(LOG_TOPIC, "NTP не отвечает");
            update_timer.setInterval(NPT_MISTAKE_UPDATE_PERIOD_MS);
            update_timer.reset();
            ntp_state = idle;
        }
        yield();
        break;
    }
}
//-------------------------------------------------------------------------------

static bool sendNTPpacket(const IPAddress &address, uint16_t port)
{
    if (!udp.beginPacket(address, port))
        return false;

    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    return udp.endPacket() == 1;
}
//-------------------------------------------------------------------------------

String getFormattedTime()
{
    return String(hour()) + ":" + String(minute()) + ":" + String(second());
}
//-------------------------------------------------------------------------------
