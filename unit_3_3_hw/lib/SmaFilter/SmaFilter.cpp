#include "SmaFilter.h"

SmaFilter::SmaFilter(size_t windowSize) {
    _windowSize = windowSize == 0 ? 1 : (windowSize > MAX_WINDOW_SIZE ? MAX_WINDOW_SIZE : windowSize);
    _currentIndex = 0;
    _isFullWindow = false;
    _runningSum = 0;

    for (size_t i = 0; i < MAX_WINDOW_SIZE; ++i) {
        _values[i] = 0;
    }
}

void SmaFilter::addValue(int16_t value) {
    if (_isFullWindow) {
        _runningSum -= _values[_currentIndex];
    }

    _values[_currentIndex] = value;
    _runningSum += value;

    _currentIndex++;
    if (_currentIndex >= _windowSize) {
        _currentIndex = 0;
        _isFullWindow = true;
    }
}

float SmaFilter::getAverage() const {
    const size_t count = _isFullWindow ? _windowSize : _currentIndex;
    return count == 0 ? 0.0f : (float)_runningSum / count;
}
