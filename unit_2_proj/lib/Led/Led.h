#ifndef LED_H
#define LED_H

#include "driver/gpio.h"

enum class LedState { OFF, ON };

class Led {
  public:
    Led(gpio_num_t pin);
    esp_err_t init(LedState state = LedState::OFF);
    void setState(LedState state);
    void toggle();

  private:
    const gpio_num_t PIN;
    bool _level{false};
    bool _isInitialized{false};

    void setOn();
    void setOff();
};

#endif
