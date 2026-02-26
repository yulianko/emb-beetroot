#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led
{
public:
    Led(int pin);

    void setOn();
    void setOff();
    void setBlink();
    void tick();

private:
    enum Mode
    {
        OFF,
        ON,
        BLINK
    };

    int _pin;

    Mode _mode;

    unsigned long _lastBlink = 0;
    bool _blinkState = false;
};

#endif