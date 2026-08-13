#include "timer.h"

#include <Arduino.h>

Timer::Timer(uint32_t interval_ms, bool ready_now) :
    interval_ms_(interval_ms), started_ms_(millis())
{
    if (ready_now)
        started_ms_ -= interval_ms_;
}
//-------------------------------------------------------------------------------

bool Timer::elapsed()
{
    uint32_t now = millis();
    // Вычитание беззнаковых чисел корректно переживает переполнение millis().
    // Unsigned subtraction remains correct when millis() rolls over.
    if ((uint32_t)(now - started_ms_) < interval_ms_)
        return false;

    started_ms_ = now;
    return true;
}
//-------------------------------------------------------------------------------

void Timer::reset()
{
    started_ms_ = millis();
}
//-------------------------------------------------------------------------------

void Timer::setInterval(uint32_t interval_ms)
{
    interval_ms_ = interval_ms;
}
