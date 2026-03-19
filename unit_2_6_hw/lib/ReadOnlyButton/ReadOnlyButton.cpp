#include "ReadOnlyButton.h"

ReadOnlyButton::ReadOnlyButton(gpio_num_t pin) : PIN(pin) {
}

void ReadOnlyButton::init() {
    if (_isInitialized) {
        return;
    }

    gpio_config_t config = {
        .pin_bit_mask = (1ULL << PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&config);

    _isInitialized = true;
}

bool ReadOnlyButton::isPressedNow() {
    if (!_isInitialized) {
        return false;
    }

    return gpio_get_level(PIN) == 1;
}
