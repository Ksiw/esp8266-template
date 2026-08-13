#include "hardware_timer.h"

#include <Arduino.h>

HardwareTimer::~HardwareTimer()
{
    stop();
}
//-------------------------------------------------------------------------------

bool HardwareTimer::begin(uint32_t period_us, Handler handler)
{
    return begin(period_us, handler, Config());
}
//-------------------------------------------------------------------------------

bool HardwareTimer::begin(uint32_t period_us, Handler handler, const Config &config)
{
    if (handler == nullptr || period_us == 0)
        return false;

    config_         = config;
    uint8_t divider = config_.divider == Divider::Div1    ? TIM_DIV1
                      : config_.divider == Divider::Div16 ? TIM_DIV16
                                                          : TIM_DIV256;
    timer1_disable();
    timer1_isr_init();
    timer1_attachInterrupt(handler);
    timer1_enable(divider, config_.interrupt == Interrupt::Edge ? TIM_EDGE : TIM_LEVEL,
                  config_.mode == Mode::Periodic ? TIM_LOOP : TIM_SINGLE);

    running_ = true;
    if (setPeriod(period_us))
        return true;

    stop();
    return false;
}
//-------------------------------------------------------------------------------

bool HardwareTimer::setPeriod(uint32_t period_us)
{
    if (!running_ || period_us == 0)
        return false;

    period_us_ = period_us;
    // Timer1 принимает количество тактов, а API — микросекунды.
    // Timer1 accepts clock ticks while the API uses microseconds.
    uint32_t ticks_per_us = config_.divider == Divider::Div1 ? 80 : config_.divider == Divider::Div16 ? 5 : 0;
    uint32_t ticks        = ticks_per_us == 0 ? (period_us_ * 5UL) / 16UL : period_us_ * ticks_per_us;
    if (ticks == 0 || ticks > 8388607UL)
        return false;

    timer1_write(ticks);
    return true;
}
//-------------------------------------------------------------------------------

void HardwareTimer::stop()
{
    if (!running_)
        return;

    timer1_disable();
    timer1_detachInterrupt();
    running_ = false;
}
//-------------------------------------------------------------------------------
