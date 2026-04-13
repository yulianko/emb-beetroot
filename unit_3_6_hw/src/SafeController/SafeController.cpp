#include "SafeController.h"

SafeController::SafeController(std::unique_ptr<SafeStateMachine> safe) : _safe(std::move(safe)) {
}

SafeController::Command SafeController::begin(const uint8_t* secretCode, size_t codeSize, uint8_t attemptsThreshold) {
    _safe->setCode(secretCode, codeSize, attemptsThreshold);
    _safe->confirm();
    return Command::NEW_ATTEMPT;
}

SafeController::Command SafeController::onIncrement(uint8_t delta) {
    _safe->increment(delta);
    return Command::DIGIT_UPDATED;
}

SafeController::Command SafeController::onDirectionChanged() {
    SafeStateMachine::StateInfo info = _safe->confirm();

    switch (info.state) {
        case SafeStateMachine::GRANTED:
            return Command::GRANTED;

        case SafeStateMachine::WRONG_CODE:
            return startNextAttempt() == Command::LOCKED ? Command::LOCKED : Command::NEW_ATTEMPT_AFTER_WRONG;

        default:
            return Command::DIGIT_CONFIRMED;
    }
}

SafeController::Command SafeController::onNewAttempt() {
    return startNextAttempt();
}

SafeController::Command SafeController::startNextAttempt() {
    SafeStateMachine::StateInfo info = _safe->newAttempt();
    return info.state == SafeStateMachine::LOCKED ? Command::LOCKED : Command::NEW_ATTEMPT;
}

uint8_t SafeController::currentDigitValue() const {
    SafeStateMachine::StateInfo info = _safe->getState();
    return info.enteredCode[info.currentDigitPosition];
}

uint8_t SafeController::currentDigitPosition() const {
    SafeStateMachine::StateInfo info = _safe->getState();
    return info.currentDigitPosition;
}

uint8_t SafeController::attemptNumber() const {
    SafeStateMachine::StateInfo info = _safe->getState();
    return info.attemptNumber;
}

bool SafeController::isEndOfWork() const {
    SafeStateMachine::StateInfo info = _safe->getState();
    return info.state == SafeStateMachine::GRANTED || info.state == SafeStateMachine::LOCKED;
}
