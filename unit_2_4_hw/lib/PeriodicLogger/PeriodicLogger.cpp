#include <Arduino.h>
#include "PeriodicLogger.h"

PeriodicLogger::PeriodicLogger(unsigned long interval) : _interval(interval) {}

void PeriodicLogger::tick()
{
    unsigned long currentMillis = millis();
    if (currentMillis - _lastWaitMessageAt >= _interval)
    {
        Serial.printf(".");
        _lastWaitMessageAt = currentMillis;
    }
}
