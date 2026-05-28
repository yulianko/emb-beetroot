
#ifndef QUADRATUREPCNT_H
#define QUADRATUREPCNT_H

#include <cstdint>

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

class QuadraturePcnt {
  public:
    enum Signal {
        LOW = 0,
        HIGH = 1,
    };

    QuadraturePcnt(gpio_num_t clk, gpio_num_t dt);
    esp_err_t init(int16_t maxValue, int16_t minValue, uint16_t debounceNs);
    esp_err_t tick(int16_t* position, Signal* clkState, Signal* dtState);
    esp_err_t reset();

  private:
    const gpio_num_t PIN_CLK;
    const gpio_num_t PIN_DT;

    pcnt_unit_handle_t _unit{};
};

#endif
