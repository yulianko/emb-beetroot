#ifndef PERIODIC_LOGGER_H
#define PERIODIC_LOGGER_H

#include <cstdint>

#include "esp_timer.h"

class PeriodicLogger {
  public:
    PeriodicLogger(uint32_t intervalMs);
    void tick();

  private:
    const uint32_t INTERVAL_US;
    int64_t _lastTickAt = 0;
};

#endif
