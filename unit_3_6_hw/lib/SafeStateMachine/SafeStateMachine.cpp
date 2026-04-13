#include "SafeStateMachine.h"

#include <cstring>

SafeStateMachine::StateInfo SafeStateMachine::setCode(const uint8_t* secretCode,
                                                      size_t codeSize,
                                                      uint8_t attemptsThreshold) {
    if (codeSize == 0 || codeSize > MAX_CODE_SIZE || attemptsThreshold == 0) {
        _state = IDLE;
        _codeSize = 0;
        _currentDigitPosition = 0;
        memset(_enteredCode, 0, sizeof(_enteredCode));
        return getState();
    }

    _state = IDLE;
    memset(_enteredCode, 0, sizeof(_enteredCode));
    _currentDigitPosition = 0;
    _attemptNumber = 0;
    _attemptsThreshold = attemptsThreshold;

    _codeSize = codeSize;
    memset(_secretCode, 0, sizeof(_secretCode));
    memcpy(_secretCode, secretCode, codeSize);

    return getState();
}

SafeStateMachine::StateInfo SafeStateMachine::newAttempt() {
    switch (_state) {
        case WRONG_CODE:
        case ENTERING:
            if (_attemptNumber < _attemptsThreshold - 1) {
                _state = ENTERING;
                memset(_enteredCode, 0, sizeof(_enteredCode));
                _currentDigitPosition = 0;
                _attemptNumber++;
            } else {
                _state = LOCKED;
            }

            break;

        case LOCKED:
        case GRANTED:
        case IDLE:
        default:
            break;
    }

    return getState();
}

SafeStateMachine::StateInfo SafeStateMachine::increment(uint8_t count) {
    switch (_state) {
        case ENTERING:
            _enteredCode[_currentDigitPosition] = (_enteredCode[_currentDigitPosition] + count) % 10;
            break;
        case IDLE:
        case LOCKED:
        case WRONG_CODE:
        case GRANTED:
        default:
            break;
    }

    return getState();
}

SafeStateMachine::StateInfo SafeStateMachine::confirm() {
    switch (_state) {
        case IDLE:
            _state = ENTERING;
            _currentDigitPosition = 0;
            _enteredCode[_currentDigitPosition] = 0;
            break;
        case ENTERING:
            if (_currentDigitPosition < static_cast<int8_t>(_codeSize) - 1) {
                _currentDigitPosition++;
                _enteredCode[_currentDigitPosition] = 0;
            } else {
                if (memcmp(_secretCode, _enteredCode, _codeSize) == 0) {
                    _state = GRANTED;
                } else {
                    _state = WRONG_CODE;
                }
            }
            break;
        case GRANTED:
        case WRONG_CODE:
        case LOCKED:
        default:
            break;
    }

    return getState();
}

SafeStateMachine::StateInfo SafeStateMachine::getState() const {
    StateInfo info;
    info.state = _state;
    memcpy(info.enteredCode, _enteredCode, _codeSize);
    info.codeSize = _codeSize;
    info.currentDigitPosition = _currentDigitPosition;
    info.attemptNumber = _attemptNumber;
    return info;
}
