
#ifndef QUADRATUREPCNT_H
#define QUADRATUREPCNT_H

#include <cstdint>

#include "IQuadraturePcnt.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

class QuadraturePcnt : public IQuadraturePcnt {
  public:
    QuadraturePcnt(gpio_num_t clk, gpio_num_t dt);
    esp_err_t init(int16_t maxValue, int16_t minValue, uint16_t debounceNs) override;
    esp_err_t tick(int16_t* position, Signal* clkState, Signal* dtState) override;
    esp_err_t reset() override;

  private:
    const gpio_num_t PIN_CLK;
    const gpio_num_t PIN_DT;

    pcnt_unit_handle_t _unit{};
};

#endif
