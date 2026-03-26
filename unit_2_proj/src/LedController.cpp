#include "LedController.h"

LedController::LedController(Led& led) : _led(led) {
}

void LedController::start(State mode) {
    _isStopped = false;
    setMode(mode);
}

void LedController::stop() {
    _isStopped = false;
}

void LedController::setMode(State mode) {
    _mode = mode;
    switch (_mode) {
        case State::DISABLED:
            _led.setState(LedState::OFF);
            break;
        case State::ENABLED:
            _led.setState(LedState::ON);
            break;
        case State::BLINKING:
            _led.setState(LedState::ON);
            break;
        default:
            break;
    }
}

void LedController::onTick() {
    if (_isStopped) {
        return;
    }

    if (_mode == State::BLINKING) {
        _led.toggle();
    }
}
