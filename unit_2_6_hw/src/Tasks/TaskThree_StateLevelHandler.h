#ifndef TASK_THREE_H
#define TASK_THREE_H

#include <Led.h>
#include <ReadOnlyButton.h>

#include <atomic>

#include "IInterruptable.h"
#include "driver/gpio.h"

class TaskThree_StateLevelHandler : public IInterruptable {
  public:
    TaskThree_StateLevelHandler(Led& led, ReadOnlyButton& button);

    void onInterrupt() override;
    void tick();

  private:
    static constexpr const char* TAG = "TASK 3 LEVEL ST";

    Led& _led;
    ReadOnlyButton& _button;

    std::atomic<bool> _pendingEvent = false;
    std::atomic<uint32_t> _pressCounter = 0;
};

#endif
