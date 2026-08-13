#pragma once

#include <stdint.h>

/*
  Обёртка над единственным пользовательским аппаратным таймером ESP8266 — Timer1.
  Timer1 нельзя использовать одновременно с analogWrite(), поскольку PWM ядра занимает его.
*/

/*
  Wrapper for the only ESP8266 hardware timer available to user code — Timer1.
  Timer1 cannot be used together with analogWrite(), because core PWM occupies it.
*/
class HardwareTimer {
public:
    using Handler = void (*)();

    enum class Divider : uint8_t {
        Div1,
        Div16,
        Div256,
    };

    enum class Interrupt : uint8_t {
        Edge,
        Level,
    };

    enum class Mode : uint8_t {
        OneShot,
        Periodic,
    };

    struct Config {
        Divider divider = Divider::Div256;
        Interrupt interrupt = Interrupt::Edge;
        Mode mode = Mode::Periodic;
    };

    HardwareTimer() = default;
    ~HardwareTimer();

    HardwareTimer(const HardwareTimer &)            = delete;
    HardwareTimer &operator=(const HardwareTimer &) = delete;

    /*
      Handler выполняется в прерывании: не используйте delay(), MQTT, Serial и выделение памяти.
    */

    /*
      Handler runs in an interrupt: do not use delay(), MQTT, Serial, or memory allocation.
    */
    // Возвращает false для нулевого/слишком длинного периода или пустого handler.
    // Returns false for a zero/too-long period or a null handler.
    bool begin(uint32_t period_us, Handler handler);
    bool begin(uint32_t period_us, Handler handler, const Config &config);
    bool setPeriod(uint32_t period_us);
    void stop();

private:
    Config config_;
    uint32_t period_us_ = 0;
    bool running_ = false;
};
//-------------------------------------------------------------------------------
