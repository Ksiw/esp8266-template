#pragma once

#include <stdint.h>

class Timer {
public:
    explicit Timer(uint32_t interval_ms, bool ready_now = false);

    // Проверяет интервал и при успехе начинает отсчёт заново.
    // Checks the interval and restarts it on success.
    bool elapsed();
    void reset();
    void setInterval(uint32_t interval_ms);

private:
    uint32_t interval_ms_;
    uint32_t started_ms_;
};
