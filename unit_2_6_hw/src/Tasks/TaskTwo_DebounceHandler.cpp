#include "TaskTwo_DebounceHandler.h"

#include "esp_log.h"
#include "esp_timer.h"

TaskTwo_DebounceHandler::TaskTwo_DebounceHandler(Led& led) : _led(led) {
}

void TaskTwo_DebounceHandler::onInterrupt() {
    int64_t now = esp_timer_get_time();

    if ((now - _lastInterruptTime) < DEBOUNCE_MS * 1000) {
        return;
    }

    _lastInterruptTime = now;
    _pendingEvent = true;
    _buttonPressesCounter++;
}

void TaskTwo_DebounceHandler::tick() {
    if (_pendingEvent.exchange(false)) {
        ESP_LOGI(TAG, "Button Pressed: %d", _buttonPressesCounter.load());
        _led.toggle();
    }
}
