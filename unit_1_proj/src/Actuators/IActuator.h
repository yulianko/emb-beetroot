#ifndef IACTUATOR_H
#define IACTUATOR_H

class IActuator
{
public:
    virtual void setOn() = 0;
    virtual void setOff() = 0;
};

#endif