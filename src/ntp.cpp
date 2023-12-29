#include <Arduino.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "defines.h"
#include "wifi.h"
#include "mqtt.h"
#include "ntp.h"

typedef enum
{
    idle,
    ready_to_start,
    starting,
    done,
    timeout,
} ntp_state_t;

#define NTP_PACKET_SIZE 48
#define NTP_LOCAL_PORT 2390

static uint32_t next_update_ms = 0;
static WiFiUDP udp;
static uint8_t packetBuffer[48] = {0};

static void ntp_time_process();
static void sendNTPpacket(const char *address, const int port);
String getFormattedTime();
static bool f_time_sets = false;
static bool f_init_done = false;
static ntp_state_t ntp_state = idle;
static uint32_t beginWait = 0;

void ntp_init()
{
    if (f_init_done == true)
        return;
    f_init_done = true;
    WRITE_INFO("Инициализация NTP...", "\n");
    udp.begin(NTP_LOCAL_PORT);
    configTime(0, 0, LOCAL_SERVER_IP);
    // setSyncProvider(ntp_time_process);
    // setSyncInterval(NPT_SET_UPDATE_PERIOD);
}

void ntp_process()
{
    if (get_wifi_connect_status())
    {
        if (f_time_sets == false)
        {
            if (millis() > next_update_ms && ntp_state == idle)
            {
                WRITE_INFO("Попытка обновления времени...", "\n");
                mqttPrintf(LOG_TOPIC, "Попытка обновления времени: ");
                ntp_state = ready_to_start;
            }
            ntp_time_process();
        }
    }
}

static void ntp_time_process()
{
    switch (ntp_state)
    {
    case idle:
        return;

    case ready_to_start:
        WRITE_INFO("Попытка запроса NTP...", "\n");
        sendNTPpacket(LOCAL_SERVER_IP, NTP_DEFAULT_LOCAL_PORT);
        beginWait = millis() + NTP_WAIT_UPDATE_MS;
        ntp_state = starting;
        break;

    case starting:
        while (udp.parsePacket() > 0)
        {
            int size = udp.read(packetBuffer, NTP_PACKET_SIZE);
            if (size == NTP_PACKET_SIZE)
            {
                setSyncInterval(NPT_UPDATE_PERIOD_MS);
                WRITE_INFO("Принят NTP ответ", "\n");
                unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
                unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
                time_t secsSince1900 = highWord << 16 | lowWord;
                secsSince1900 = secsSince1900 - 2208988800UL + NPT_TIME_ZONE * SECS_PER_HOUR;
                setTime(secsSince1900);
                ntp_state = done;
            }
        }

        if (millis() >= beginWait)
        {
            WRITE_INFO("NTP не отвечает", "\n");
            mqttPrintf(LOG_TOPIC, "NTP не отвечает");
            next_update_ms = millis() + NPT_MISTAKE_UPDATE_PERIOD_MS;
            ntp_state = done;
        }
        break;

    case done:
        WRITE_INFO("Время: ", getFormattedTime(), "\n");
        mqttPrintf(TIME_TOPIC, getFormattedTime().c_str());
        next_update_ms = millis() + NPT_UPDATE_PERIOD_MS;
        ntp_state = idle;
        break;

    default:
        break;
    }
}

void sendNTPpacket(const char *address, const int port)
{
    udp.beginPacket(address, port);
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

String getFormattedTime()
{
    return String(hour()) + ":" + String(minute()) + ":" + String(second());
}
