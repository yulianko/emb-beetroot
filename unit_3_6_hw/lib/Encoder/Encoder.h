#ifndef Encoder_H
#define Encoder_H
#include <memory>

#include "IQuadraturePcnt.h"

class Encoder2 {
  public:
    enum Direction {
        NONE = 0,
        CW = 1,
        CCW = 2,
    };

    struct State {
        int16_t rawPosition{};
        IQuadraturePcnt::Signal clkState{};
        IQuadraturePcnt::Signal dtState{};
        Direction direction{NONE};
        int16_t detentPosition{};
        int8_t accumulatedDelta{};
        bool directionChanged{};
    };

    static constexpr const char* TAG = "Encoder";

    Encoder2(std::unique_ptr<IQuadraturePcnt> quadraturePcnt);

    esp_err_t init(int16_t maxValue, int16_t minValue, uint16_t debounceNs);

    esp_err_t tick(State* state);

    int8_t consumeAccumulatedDelta();

    esp_err_t reset();

  private:
    std::unique_ptr<IQuadraturePcnt> _quadraturePcnt;

    int16_t _lastRawPosition{};
    Direction _lastDirection{};
    int8_t _accumulatedDelta{};
};

#endif
