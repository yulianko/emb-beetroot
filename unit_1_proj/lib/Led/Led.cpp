#include "Led.h"

Led::Led(uint8_t pin) : _pin(pin), _mode(OFF)
{
    pinMode(_pin, OUTPUT);
    setOff();
}

void Led::setOn()
{
    if (_mode == ON)
    {
        return;
    }

    _mode = ON;
    digitalWrite(_pin, HIGH);
}

void Led::setOff()
{
    if (_mode == OFF)
    {
        return;
    }

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