#include "TaskFour_PollingDebounce.h"

#include "esp_log.h"
#include "esp_timer.h"

TaskFour_PollingDebounce::TaskFour_PollingDebounce(Led& led, ReadOnlyButton& button) : _led(led), _button(button) {
}

void TaskFour_PollingDebounce::tick() {
    bool pressed = _button.isPressedNow();
    int64_t now = esp_timer_get_time();

    switch (_state) {
        case State::IDLE:
            if (pressed) {
                _debounceStart = now;
                _state = State::DEBOUNCING;
            }
            break;

        case State::DEBOUNCING:
            if (!pressed) {
                _state = State::IDLE;
            } else if ((now - _debounceStart) >= DEBOUNCE_MS * 1000) {
                _pressCounter++;
                ESP_LOGI(TAG, "Button Pressed: %d", _pressCounter);
                _led.toggle();
                _state = State::PRESSED;
            }
            break;

        case State::PRESSED:
            if (!pressed) {
                _state = State::IDLE;
            }
            break;
    }
}
