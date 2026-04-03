#pragma once
#include <cstddef>
#include <cstdint>

class SmaFilter {
  public:
    static const size_t MAX_WINDOW_SIZE = 20;

    SmaFilter(size_t windowSize);

    void addValue(int16_t value);
    float getAverage() const;

  private:
    int16_t _values[MAX_WINDOW_SIZE] = {0};
    int32_t _runningSum = 0;

    size_t _windowSize;
    size_t _currentIndex = 0;
    bool _isFullWindow = false;
};
