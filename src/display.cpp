#include "defines.h"
#include "display.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "mqtt.h"
#include "sensor.h"
#include "ESP8266TrueRandom.h"
#include <timer.h>

static Adafruit_SSD1306 display(LCD_WIDTH, LCD_HEIGHT, &Wire, OLED_RESET);

typedef struct {
    bool f_noise;
    bool f_show_temperature;
    bool f_show_humidity;
    bool f_show_started;
} display_t;

static display_t d;
static Timer noise_timer(LCD_NOISE_MS);
static Timer show_data_timer(LCD_TEMPERATURE_MS);
static void check_pause();

//-------------------------------------------------------------------------------
void display_init()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    d.f_noise = true;
    noise_timer.reset();
    d.f_show_temperature = false;
    d.f_show_humidity    = false;
    d.f_show_started     = false;
    display.setTextSize(2);
}
//-------------------------------------------------------------------------------

void display_process()
{
    check_pause();
    if (d.f_noise == true) {
        display.clearDisplay();
        display.display();
        display.drawPixel(ESP8266TrueRandom.random(1, LCD_WIDTH), ESP8266TrueRandom.random(1, LCD_HEIGHT), WHITE);
        display.display();
    } else if (d.f_show_temperature == true && d.f_show_started == false) {
        d.f_show_started = true;
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 5);
        display.setTextSize(2);
        display.print(get_temperature(), 1);
        display.print(F("C"));
        display.display();
    } else if (d.f_show_humidity == true && d.f_show_started == false) {
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
    if (d.f_noise == true) {
        if (noise_timer.elapsed()) {
            d.f_noise            = false;
            d.f_show_temperature = true;
            show_data_timer.setInterval(LCD_TEMPERATURE_MS);
            show_data_timer.reset();
        }
    } else if (d.f_show_temperature == true) {
        if (show_data_timer.elapsed()) {
            d.f_show_temperature = false;
            d.f_show_humidity    = true;
            show_data_timer.setInterval(LCD_HUMIDITY_MS);
            show_data_timer.reset();
            d.f_show_started = false;
        }
    } else if (d.f_show_humidity == true) {
        if (show_data_timer.elapsed()) {
            d.f_show_humidity = false;
            d.f_noise         = true;
            noise_timer.reset();
            d.f_show_started = false;
        }
    }
}
//-------------------------------------------------------------------------------
