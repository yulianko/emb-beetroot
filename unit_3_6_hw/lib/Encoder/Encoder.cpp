#include "Encoder.h"

#include <cstdlib>

Encoder2::Encoder2(std::unique_ptr<IQuadraturePcnt> quadraturePcnt) : _quadraturePcnt(std::move(quadraturePcnt)) {
}

esp_err_t Encoder2::init(int16_t maxValue, int16_t minValue, uint16_t debounceNs) {
    return _quadraturePcnt->init(maxValue, minValue, debounceNs);
}

esp_err_t Encoder2::tick(State* state) {
    esp_err_t err = _quadraturePcnt->tick(&state->rawPosition, &state->clkState, &state->dtState);
    if (err != ESP_OK) {
        return err;
    }

    state->directionChanged = false;
    Direction previousDirection = _lastDirection;

    int delta = state->rawPosition - _lastRawPosition;
    int8_t steps = 0;
    if (abs(delta) >= IQuadraturePcnt::PHASES_COUNT) {
        steps = delta / IQuadraturePcnt::PHASES_COUNT;
        _lastRawPosition += steps * IQuadraturePcnt::PHASES_COUNT;
        Direction currentDirection = (delta > 0) ? CW : CCW;
        if (previousDirection != NONE && currentDirection != previousDirection) {
            state->directionChanged = true;
        }
        _lastDirection = currentDirection;
        _accumulatedDelta += steps;
    }

    state->direction = _lastDirection;
    state->detentPosition = _lastRawPosition / IQuadraturePcnt::PHASES_COUNT;
    state->accumulatedDelta = _accumulatedDelta;

    return ESP_OK;
}

int8_t Encoder2::consumeAccumulatedDelta() {
    int8_t delta = _accumulatedDelta;
    _accumulatedDelta = 0;
    return delta;
}

esp_err_t Encoder2::reset() {
    _lastRawPosition = 0;
    _lastDirection = NONE;
    _accumulatedDelta = 0;
    _quadraturePcnt->reset();
    return ESP_OK;
}
