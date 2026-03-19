#ifndef TASK_FOUR_H
#define TASK_FOUR_H

#include "Led.h"
#include "ReadOnlyButton.h"

class TaskFour_PollingDebounce {
  public:
    TaskFour_PollingDebounce(Led& led, ReadOnlyButton& button);
    void tick();

  private:
    enum class State {
        IDLE,
        DEBOUNCING,
        PRESSED,
    };

    static constexpr const char* TAG = "TASK 4 POOL&DEB";
    static constexpr uint32_t DEBOUNCE_MS = 50;

    Led& _led;
    ReadOnlyButton& _button;

    State _state = State::IDLE;
    int64_t _debounceStart = 0;
    uint32_t _pressCounter = 0;
};

#endif
