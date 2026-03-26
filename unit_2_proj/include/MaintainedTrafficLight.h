#ifndef MAINTAINED_TRAFFIC_LIGHT_H
#define MAINTAINED_TRAFFIC_LIGHT_H

#include "ITrafficLight.h"
#include "LedController.h"

class MaintainedTrafficLight : public ITrafficLight {
  public:
    MaintainedTrafficLight(LedController& green, LedController& yellow, LedController& red);

    void start() override;
    void stop() override;
    void onTick() override;

  private:
    LedController& _green;
    LedController& _yellow;
    LedController& _red;

    bool _isStopped{true};

    uint32_t _currentTick{0};
};

#endif
