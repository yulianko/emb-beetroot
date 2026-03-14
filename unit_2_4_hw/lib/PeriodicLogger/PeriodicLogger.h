#ifndef PERIODIC_LOGGER_H
#define PERIODIC_LOGGER_H

class PeriodicLogger
{
private:
    unsigned long _interval;
    unsigned long _lastWaitMessageAt = 0;

public:
    PeriodicLogger(unsigned long interval);
    void tick();
};

#endif
