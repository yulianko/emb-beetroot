#include "TrafficLight.h"

#include "ITrafficLight.h"
#include "MaintainedTrafficLight.h"
#include "TrafficLightStateMachine.h"

TrafficLight::TrafficLight(ITrafficLight& normal, ITrafficLight& manitained)
    : _normal(normal), _maintaned(manitained), _current(&normal) {
}

void TrafficLight::start() {
    _current->start();
}

void TrafficLight::stop() {
    _current->stop();
}

void TrafficLight::onTick() {
    _current->onTick();
}

void TrafficLight::toggleMode() {
    _current->stop();
    _current = _current == &_normal ? &_maintaned : &_normal;
    _current->start();
}
