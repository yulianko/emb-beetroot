#include "MaintainedTrafficLight.h"

#include "LedController.h"

MaintainedTrafficLight::MaintainedTrafficLight(LedController& green, LedController& yellow, LedController& red)
    : _green(green), _yellow(yellow), _red(red) {
}

void MaintainedTrafficLight::start() {
    _isStopped = false;

    _green.stop();
    _yellow.start();
    _red.stop();

    _green.setMode(LedController::State::DISABLED);
    _yellow.setMode(LedController::State::BLINKING);
    _red.setMode(LedController::State::DISABLED);
}

void MaintainedTrafficLight::stop() {
    _isStopped = true;

    _green.stop();
    _yellow.stop();
    _red.stop();

    _green.setMode(LedController::State::DISABLED);
    _yellow.setMode(LedController::State::DISABLED);
    _red.setMode(LedController::State::DISABLED);
}

void MaintainedTrafficLight::onTick() {
    if (_isStopped) {
        return;
    }

    _yellow.onTick();
}
