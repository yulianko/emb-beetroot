#include "Led.h"
Led::Led(gpio_num_t pin) : PIN(pin) {
}

esp_err_t Led::init(LedState state) {
    if (_isInitialized) {
        return ESP_FAIL;
    }

    gpio_config_t ledConfig = {
        .pin_bit_mask = (1ULL << PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t error = gpio_config(&ledConfig);
    if (error != ESP_OK) {
        return error;
    }

    _isInitialized = true;

    setState(state);

    return ESP_OK;
}

void Led::setState(LedState state) {
    if (!_isInitialized) {
        return;
    }

    switch (state) {
        case LedState::ON:
            setOn();
            break;

        case LedState::OFF:
            setOff();
            break;

        default:
            break;
    }
}

void Led::toggle() {
    if (!_isInitialized) {
        return;
    }

    _level = !_level;
    gpio_set_level(PIN, _level ? 1 : 0);
}

void Led::setOn() {
    if (!_isInitialized) {
        return;
    }

    _level = true;
    gpio_set_level(PIN, 1);
}

void Led::setOff() {
    if (!_isInitialized) {
        return;
    }

    _level = false;
    gpio_set_level(PIN, 0);
}
