#ifndef TASK_ONE_H
#define TASK_ONE_H

#include <Led.h>

#include <atomic>

#include "IInterruptable.h"

class TaskOne_RawInterruptHandler : public IInterruptable {
  public:
    TaskOne_RawInterruptHandler(Led& led);

    void onInterrupt() override;

    void tick();

  private:
    static constexpr const char* TAG = "TASK 1 RAW INTR";

    std::atomic<uint32_t> _interruptsCounter = 0;
    std::atomic<bool> _interruptHandled = true;

    Led& _led;
};

#endif
