#ifndef READ_ONLY_BUTTON_H
#define READ_ONLY_BUTTON_H

#include "driver/gpio.h"

class ReadOnlyButton {
  public:
    ReadOnlyButton(gpio_num_t pin);
    void init();
    bool isPressedNow();

  private:
    const gpio_num_t PIN;
    bool _isInitialized = false;
};

#endif
