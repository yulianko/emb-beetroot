#include "LdrSensor.h"

LdrSensor::LdrSensor(uint8_t pin, float uRef, uint8_t resolution, float maxValue, adc_attenuation_t attenuation)
    : _pin(pin), _uref(uRef), _resolution(resolution), _maxValue(maxValue), _attenuation(attenuation)
{
    pinMode(_pin, INPUT);
    analogReadResolution(_resolution);
    analogSetPinAttenuation(_pin, _attenuation);
}

void LdrSensor::setThreshold(float threshold, float hysteresis)
{
    _threshold = threshold;
    _hysteresis = hysteresis;
}

void LdrSensor::attachChange(LdrSensorCallback handler)
{
    _onChange = handler;
}

bool LdrSensor::isDark()
{
    float val = _read();
    if (val < (_threshold - _hysteresis))
    {
        _lastState = true;
    }
    else
    {
        if (val > (_threshold + _hysteresis))
        {
            _lastState = false;
        }
    }

    return _lastState;
}

void LdrSensor::tick()
{
    bool prevState = _lastState;
    bool newState = isDark();
    if (newState != prevState && _onChange)
    {
        _onChange(newState);
    }
}

float LdrSensor::_read()
{
    uint16_t raw = analogRead(_pin);
    return (raw / _maxValue) * _uref;
}