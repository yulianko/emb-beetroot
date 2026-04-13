#ifndef SAFESTATEMACHINE_H
#define SAFESTATEMACHINE_H

#include <cstddef>
#include <cstdint>

class SafeStateMachine {
  public:
    enum State { IDLE, ENTERING, WRONG_CODE, GRANTED, LOCKED };

    static constexpr size_t MAX_CODE_SIZE = 8;

    struct StateInfo {
        State state{};
        uint8_t enteredCode[MAX_CODE_SIZE]{};
        size_t codeSize{};
        uint8_t currentDigitPosition{};
        uint8_t attemptNumber{};
    };

    StateInfo setCode(const uint8_t* secretCode, size_t codeSize, uint8_t attemptsThreshold);
    StateInfo increment(uint8_t count);
    StateInfo confirm();
    StateInfo newAttempt();
    StateInfo getState() const;

  private:
    State _state{};

    uint8_t _secretCode[MAX_CODE_SIZE]{};
    uint8_t _enteredCode[MAX_CODE_SIZE]{};
    size_t _codeSize{};

    uint8_t _currentDigitPosition{};

    uint8_t _attemptsThreshold{};
    uint8_t _attemptNumber{};
};

#endif
