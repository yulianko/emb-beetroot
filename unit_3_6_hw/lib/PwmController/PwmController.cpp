#include "PwmController.h"

PwmController::PwmController(int pin, ledc_channel_t channel, ledc_timer_t timer, ledc_mode_t speedMode)
    : PIN(pin), CHANNEL(channel), TIMER(timer), SPEED_MODE(speedMode) {
}

esp_err_t PwmController::init(uint32_t frequencyHz, ledc_timer_bit_t resolution, uint32_t initialDuty) {
    if (_initialized) {
        return ESP_OK;
    }

    _frequencyHz = frequencyHz;
    _maxDutyCycle = (1u << resolution) - 1;
    if (initialDuty > _maxDutyCycle) {
        initialDuty = _maxDutyCycle;
    }

    ledc_timer_config_t timerConfig = {
        .speed_mode = SPEED_MODE,
        .duty_resolution = resolution,
        .timer_num = TIMER,
        .freq_hz = frequencyHz,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t err = ledc_timer_config(&timerConfig);
    if (err != ESP_OK) {
        return err;
    }

    ledc_channel_config_t channelConfig = {
        .gpio_num = PIN,
        .speed_mode = SPEED_MODE,
        .channel = CHANNEL,
        .timer_sel = TIMER,
        .duty = initialDuty,
        .hpoint = 0,
    };
    err = ledc_channel_config(&channelConfig);
    if (err != ESP_OK) {
        return err;
    }

    _initialized = true;
    return ESP_OK;
}

esp_err_t PwmController::setDuty(uint32_t duty) {
    if (!_initialized) {
        return ESP_FAIL;
    }

    if (duty > _maxDutyCycle) {
        duty = _maxDutyCycle;
    }

    esp_err_t err = ledc_set_duty(SPEED_MODE, CHANNEL, duty);
    if (err != ESP_OK) {
        return err;
    }

    return ledc_update_duty(SPEED_MODE, CHANNEL);
}

uint32_t PwmController::getMaxDutyCycle() const {
    return _maxDutyCycle;
}

uint32_t PwmController::getFrequency() const {
    return _frequencyHz;
}
