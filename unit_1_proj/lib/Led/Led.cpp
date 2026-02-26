#include "Led.h"

Led::Led(int pin) : _pin(pin), _mode(OFF)
{
    pinMode(_pin, OUTPUT);
    setOff();
}

void Led::setOn()
{
    _mode = ON;
    digitalWrite(_pin, HIGH);
}

void Led::setOff()
{
    _mode = OFF;
    digitalWrite(_pin, LOW);
}

void Led::setBlink()
{
    _mode = BLINK;
}

void Led::tick()
{
    if (_mode != BLINK)
    {
        return;
    }

    if (millis() - _lastBlink >= 500)
    {
        _lastBlink = millis();
        _blinkState = !_blinkState;
        digitalWrite(_pin, _blinkState ? HIGH : LOW);
    }
}