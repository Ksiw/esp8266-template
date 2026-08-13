#pragma once

#include <Arduino.h>
#include <timer.h>
//-------------------------------------------------------------------------------

class DebouncedInput {
public:
    explicit DebouncedInput(uint8_t pin, uint8_t mode = INPUT, uint32_t debounce_ms = 10);

    void begin();

    // Возвращает true один раз, когда новый уровень стабилен debounce_ms.
    // Returns true once when a new level has remained stable for debounce_ms.
    // Вызывайте update() часто из loop(), иначе фактический антидребезг будет дольше debounce_ms.
    // Call update() frequently from loop(), or the actual debounce time will exceed debounce_ms.
    bool update();
    bool read() const;

private:
    uint8_t pin_;
    uint8_t mode_;
    bool stable_value_;
    bool last_value_;
    Timer debounce_timer_;
};
//-------------------------------------------------------------------------------

class PulseCounter {
public:
    PulseCounter(uint8_t pin, int interrupt_mode = FALLING, uint8_t pin_mode = INPUT);

    void begin();
    uint32_t read() const;
    uint32_t readAndReset();
    void reset();

private:
    static void IRAM_ATTR onInterrupt(void *arg);

    uint8_t pin_;
    int interrupt_mode_;
    uint8_t pin_mode_;
    volatile uint32_t count_;
};
//-------------------------------------------------------------------------------

void gpio_module_init();
void gpio_module_process();
