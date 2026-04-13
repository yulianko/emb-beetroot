#ifndef SAFE_CONTROLLER_H
#define SAFE_CONTROLLER_H
#include <memory>

#include "SafeStateMachine.h"

class SafeController {
  public:
    enum class Command : uint8_t {
        NONE,
        NEW_ATTEMPT,
        NEW_ATTEMPT_AFTER_WRONG,
        DIGIT_UPDATED,
        DIGIT_CONFIRMED,
        LOCKED,
        GRANTED,
    };

    SafeController(std::unique_ptr<SafeStateMachine> safe);

    Command begin(const uint8_t* secretCode, size_t codeSize, uint8_t attemptsThreshold);
    Command onIncrement(uint8_t delta);
    Command onDirectionChanged();
    Command onNewAttempt();

    uint8_t currentDigitValue() const;
    uint8_t currentDigitPosition() const;
    uint8_t attemptNumber() const;

    bool isEndOfWork() const;

  private:
    std::unique_ptr<SafeStateMachine> _safe;
    Command startNextAttempt();
};

#endif
