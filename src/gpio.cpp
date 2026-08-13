#include "defines.h"
#include <Arduino.h>
#include <GyverIO.h>
#include "gpio.h"

DebouncedInput::DebouncedInput(uint8_t pin, uint8_t mode, uint32_t debounce_ms) :
    pin_(pin), mode_(mode), stable_value_(false), last_value_(false), debounce_timer_(debounce_ms)
{
}
//-------------------------------------------------------------------------------

void DebouncedInput::begin()
{
    gio::init(pin_);
    pinMode(pin_, mode_);
    stable_value_ = gio::read(pin_);
    last_value_   = stable_value_;
    debounce_timer_.reset();
}
//-------------------------------------------------------------------------------

bool DebouncedInput::update()
{
    bool value = gio::read(pin_);
    if (value != last_value_) {
        last_value_ = value;
        // Каждый скачок сигнала начинает отсчёт стабильных 10 мс заново.
        // Every signal transition restarts the stable 10 ms interval.
        debounce_timer_.reset();
    }

    if (value == stable_value_ || !debounce_timer_.elapsed())
        return false;

    stable_value_ = value;
    return true;
}
//-------------------------------------------------------------------------------

bool DebouncedInput::read() const
{
    return stable_value_;
}
//-------------------------------------------------------------------------------

PulseCounter::PulseCounter(uint8_t pin, int interrupt_mode, uint8_t pin_mode) :
    pin_(pin), interrupt_mode_(interrupt_mode), pin_mode_(pin_mode), count_(0)
{
}
//-------------------------------------------------------------------------------

void PulseCounter::begin()
{
    pinMode(pin_, pin_mode_);
    attachInterruptArg(pin_, onInterrupt, this, interrupt_mode_);
}
//-------------------------------------------------------------------------------

uint32_t PulseCounter::read() const
{
    noInterrupts();
    uint32_t count = count_;
    interrupts();
    return count;
}
//-------------------------------------------------------------------------------

uint32_t PulseCounter::readAndReset()
{
    noInterrupts();
    uint32_t count = count_;
    count_ = 0;
    interrupts();
    return count;
}
//-------------------------------------------------------------------------------

void PulseCounter::reset()
{
    noInterrupts();
    count_ = 0;
    interrupts();
}
//-------------------------------------------------------------------------------

void IRAM_ATTR PulseCounter::onInterrupt(void *arg)
{
    static_cast<PulseCounter *>(arg)->count_++;
}
//-------------------------------------------------------------------------------

void gpio_module_init()
{
}
//-------------------------------------------------------------------------------

void gpio_module_process()
{
}
//-------------------------------------------------------------------------------
