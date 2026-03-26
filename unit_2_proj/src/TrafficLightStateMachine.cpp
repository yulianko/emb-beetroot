#include "TrafficLightStateMachine.h"

TrafficLightStateMachine::TrafficLightStateMachine(LedController& green, LedController& yellow, LedController& red)
    : _green(green), _yellow(yellow), _red(red) {
}

const TrafficLightStateMachineConfig TrafficLightStateMachine::_transitions[] = {
    // GREEN
    {
        .ticks = 5000 / ITrafficLight::TICK_PERIOD_MS,
        .next = TrafficLightStateMachineState::BLINK_GREEN,
        .green = LedController::State::ENABLED,
        .yellow = LedController::State::DISABLED,
        .red = LedController::State::DISABLED,
    },
    // BLINK_GREEN
    {
        .ticks = 3000 / ITrafficLight::TICK_PERIOD_MS,
        .next = TrafficLightStateMachineState::YELLOW,
        .green = LedController::State::BLINKING,
        .yellow = LedController::State::DISABLED,
        .red = LedController::State::DISABLED,
    },
    // YELLOW
    {
        .ticks = 2000 / ITrafficLight::TICK_PERIOD_MS,
        .next = TrafficLightStateMachineState::RED,
        .green = LedController::State::DISABLED,
        .yellow = LedController::State::ENABLED,
        .red = LedController::State::DISABLED,
    },
    // RED
    {
        .ticks = 5000 / ITrafficLight::TICK_PERIOD_MS,
        .next = TrafficLightStateMachineState::RED_YELLOW,
        .green = LedController::State::DISABLED,
        .yellow = LedController::State::DISABLED,
        .red = LedController::State::ENABLED,
    },
    // RED_YELLOW
    {
        .ticks = 2000 / ITrafficLight::TICK_PERIOD_MS,
        .next = TrafficLightStateMachineState::GREEN,
        .green = LedController::State::DISABLED,
        .yellow = LedController::State::ENABLED,
        .red = LedController::State::ENABLED,
    },
};

void TrafficLightStateMachine::start() {
    _state = TrafficLightStateMachineState::RED;
    _currentTick = 0;
    _isStopped = false;

    _green.start();
    _yellow.start();
    _red.start();

    applyState();
}

void TrafficLightStateMachine::stop() {
    _isStopped = true;

    _green.stop();
    _yellow.stop();
    _red.stop();

    _state = TrafficLightStateMachineState::RED;
    _currentTick = 0;
    applyState();
}

void TrafficLightStateMachine::onTick() {
    if (_isStopped) {
        return;
    }

    _currentTick++;

    const TrafficLightStateMachineConfig& config = _transitions[static_cast<int>(_state)];
    if (_currentTick >= config.ticks) {
        _currentTick = 0;
        _state = config.next;
        applyState();
    }

    _green.onTick();
    _yellow.onTick();
    _red.onTick();
}

void TrafficLightStateMachine::applyState() {
    const TrafficLightStateMachineConfig& config = _transitions[static_cast<int>(_state)];
    _green.setMode(config.green);
    _yellow.setMode(config.yellow);
    _red.setMode(config.red);
}
