#ifndef TASK_TWO_H
#define TASK_TWO_H

#include <atomic>

#include "IInterruptable.h"
#include "Led.h"

class TaskTwo_DebounceHandler : public IInterruptable {
  public:
    TaskTwo_DebounceHandler(Led& led);

    void onInterrupt() override;
    void tick();

  private:
    static constexpr const char* TAG = "TASK 2 DEBOUNCE";
    static constexpr uint32_t DEBOUNCE_MS = 50;

    Led& _led;

    std::atomic<int64_t> _lastInterruptTime = 0;
    std::atomic<bool> _pendingEvent = false;
    std::atomic<int32_t> _buttonPressesCounter = 0;
};

#endif
