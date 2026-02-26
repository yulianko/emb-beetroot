#ifndef LdrSensor_H
#define LdrSensor_H

#include <Arduino.h>

typedef void (*LdrSensorCallback)(bool isDark);

class LdrSensor
{
public:
    LdrSensor(uint8_t pin, float uRef = 3.3, uint8_t resolution = 12, float maxValue = 4095.0, adc_attenuation_t attenuation = ADC_11db);

    void setThreshold(float threshold = 1.6, float hysteresis = 0.2);
    void attachChange(LdrSensorCallback newFunction);
    bool isDark();
    void tick();

private:
    uint8_t _pin;
    float _uref;
    uint8_t _resolution;
    float _maxValue;
    adc_attenuation_t _attenuation;

    float _threshold;
    float _hysteresis;

    bool _lastState;
    LdrSensorCallback _onChange = nullptr;

    float _read();
};

#endif