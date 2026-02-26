#ifndef LEDACTIATOR_H
#define LEDACTIATOR_H

#include <Led.h>
#include "IActuator.h"

class LedActuator : public IActuator
{
public:
    LedActuator(int pin);
    void setOn() override;
    void setOff() override;

private:
    Led _led;
};

#endif