
#ifndef IQUADRATUREPCNT_H
#define IQUADRATUREPCNT_H

#ifdef ESP_PLATFORM
#include <esp_err.h>
#else
typedef int esp_err_t;
#define ESP_OK 0
#endif

#include <cstdint>

class IQuadraturePcnt {
  public:
    enum Signal {
        LOW = 0,
        HIGH = 1,
    };

    static const uint8_t PHASES_COUNT = 4;

    virtual esp_err_t init(int16_t maxValue, int16_t minValue, uint16_t debounceNs) = 0;
    virtual esp_err_t tick(int16_t* position, Signal* clkState, Signal* dtState) = 0;
    virtual esp_err_t reset() = 0;
};

#endif
