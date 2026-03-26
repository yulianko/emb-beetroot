#ifndef TRAFFIC_LIGHT_STATE_MACHINE_H
#define TRAFFIC_LIGHT_STATE_MACHINE_H

#include "ITrafficLight.h"
#include "LedController.h"

enum class TrafficLightStateMachineState { GREEN, BLINK_GREEN, YELLOW, RED, RED_YELLOW };

struct TrafficLightStateMachineConfig {
    uint32_t ticks;
    TrafficLightStateMachineState next;
    LedController::State green;
    LedController::State yellow;
    LedController::State red;
};

class TrafficLightStateMachine : public ITrafficLight {
  public:
    TrafficLightStateMachine(LedController& green, LedController& yellow, LedController& red);

    void start() override;
    void stop() override;
    void onTick() override;

  private:
    static const TrafficLightStateMachineConfig _transitions[];

    LedController& _green;
    LedController& _yellow;
    LedController& _red;

    bool _isStopped{true};
    TrafficLightStateMachineState _state{TrafficLightStateMachineState::RED};

    uint32_t _currentTick{0};

    void applyState();
};

#endif
