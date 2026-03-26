#ifndef I_TRAFFIC_LIGHT_H
#define I_TRAFFIC_LIGHT_H

#include "LedController.h"

class ITrafficLight {
  public:
    static constexpr uint32_t TICK_PERIOD_MS{500};

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void onTick() = 0;
};

#endif
