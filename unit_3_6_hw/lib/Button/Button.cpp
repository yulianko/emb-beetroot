#include "Button.h"

Button::Button(gpio_num_t pin) : PIN(pin) {
}

esp_err_t Button::init() {
    if (_isInitialized) {
        return ESP_FAIL;
    }

    gpio_config_t config = {
        .pin_bit_mask = (1ULL << PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t error = gpio_config(&config);
    if (error != ESP_OK) {
        return error;
    }

    _isInitialized = true;

    return ESP_OK;
}

esp_err_t Button::tick(Event* event) {
    *event = Event::NONE;
    if (!_isInitialized) {
        return ESP_FAIL;
    }

    bool pressed = gpio_get_level(PIN) == 0;
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
            } else if ((now - _debounceStart) >= DEBOUNCE_US) {
                _state = State::PRESSED;
            }
            break;

        case State::PRESSED:
            if (!pressed) {
                *event = Event::CLICKED;
                _state = State::IDLE;
            }
            break;
    }

    return ESP_OK;
}
