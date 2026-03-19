#ifndef LED_H
#define LED_H

#include "driver/gpio.h"

enum class LedState { ON, OFF };

class Led {
  public:
    Led(gpio_num_t pin);
    void init(LedState state = LedState::OFF);
    void setState(LedState state);
    void toggle();

  private:
    const gpio_num_t PIN;
    bool _level = false;
    bool _isInitialized = false;

    void setOn();
    void setOff();
};

#endif
