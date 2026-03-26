#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Led.h>

class LedController {
  public:
    enum class State { DISABLED, ENABLED, BLINKING };

    explicit LedController(Led& led);

    void start(State mode = State::DISABLED);
    void stop();
    void setMode(State mode);
    void onTick();

  private:
    Led& _led;
    State _mode{State::DISABLED};

    bool _isStopped{true};
};

#endif
