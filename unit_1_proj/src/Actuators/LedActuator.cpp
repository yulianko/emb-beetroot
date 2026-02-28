#include "LedActuator.h"

LedActuator::LedActuator(int pin) : _led(Led(pin))
{
}

void LedActuator::setOn()
{
    _led.setOn();
}

void LedActuator::setOff()
{
    _led.setOff();
}