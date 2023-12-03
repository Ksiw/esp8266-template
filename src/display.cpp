#include "defines.h"
#include "display.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "mqtt.h"
#include "sensor.h"
#include "ESP8266TrueRandom.h"

static Adafruit_SSD1306 display(OLED_RESET);

typedef struct
{
    uint32_t noise_ms;
    uint32_t show_data_ms;
    bool f_noise;
    bool f_show_temperature;
    bool f_show_humidity;
    bool f_show_started;
} display_t;

static display_t d;
static void check_pause();

//-------------------------------------------------------------------------------
void display_init()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    d.f_noise = true;
    display.setTextSize(2);
}
//-------------------------------------------------------------------------------

void display_process()
{
    check_pause();
    if (d.f_noise == true)
    {
        display.clearDisplay();
        display.display();
        display.drawPixel(ESP8266TrueRandom.random(1, LCD_WIDTH),
                          ESP8266TrueRandom.random(1, LCD_HEIGHT), WHITE);
        display.display();
    }
    else if (d.f_show_temperature == true && d.f_show_started == false)
    {
        d.f_show_started = true;
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 5);
        display.setTextSize(2);
        display.print(get_temperature(), 1);
        display.print(F("C"));
        display.display();
    }
    else if (d.f_show_humidity == true && d.f_show_started == false)
    {
        d.f_show_started = true;
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 28);
        display.print(get_humidity(), 1);
        display.print(F("%"));
        display.display();
    }
}
//-------------------------------------------------------------------------------

static void check_pause()
{
    if (d.f_noise == true)
    {
        if (millis() > d.noise_ms)
        {
            d.f_noise = false;
            d.f_show_temperature = true;
            d.show_data_ms = millis() + LCD_TEMPERATURE_MS;
        }
    }
    else if (d.f_show_temperature == true)
    {
        if (millis() > d.show_data_ms)
        {
            d.f_show_temperature = false;
            d.f_show_humidity = true;
            d.show_data_ms = millis() + LCD_HUMIDITY_MS;
            d.f_show_started = false;
        }
    }
    else if (d.f_show_humidity == true)
    {
        if (millis() > d.show_data_ms)
        {
            d.f_show_humidity = false;
            d.f_noise = true;
            d.noise_ms = millis() + LCD_NOISE_MS;
            d.f_show_started = false;
        }
    }
}
//-------------------------------------------------------------------------------