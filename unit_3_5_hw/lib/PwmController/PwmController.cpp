#include "PwmController.h"

PwmController::PwmController(int pin, ledc_channel_t channel, ledc_timer_t timer)
    : PIN(pin), CHANNEL(channel), TIMER(timer) {
}

esp_err_t PwmController::init(uint32_t frequency,
                              uint32_t dutyCycle,
                              ledc_timer_bit_t dutyResolution,
                              ledc_mode_t speedMode) {
    if (_isInitialized) {
        return ESP_ERR_INVALID_STATE;
    }

    _speedMode = speedMode;
    _periodUs = 1'000'000 / frequency;
    _maxDutyCycle = (1 << dutyResolution) - 1;
    if (dutyCycle > _maxDutyCycle) {
        dutyCycle = _maxDutyCycle;
    }

    ledc_timer_config_t timerConfig = {
        .speed_mode = _speedMode,
        .duty_resolution = dutyResolution,
        .timer_num = TIMER,
        .freq_hz = frequency,
        .clk_cfg = LEDC_AUTO_CLK,  // Default
        .deconfigure = false,      // Default
    };

    esp_err_t err = ledc_timer_config(&timerConfig);
    if (err != ESP_OK) {
        return err;
    }

    ledc_channel_config_t channelConfig = {
        .gpio_num = PIN,
        .speed_mode = speedMode,
        .channel = CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,  // Default
        .timer_sel = TIMER,
        .duty = dutyCycle,
        .hpoint = 0,                                   // Default
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,  // Default
        .flags = {.output_invert = 0},                 // Default
    };

    err = ledc_channel_config(&channelConfig);
    if (err != ESP_OK) {
        return err;
    }

    _isInitialized = true;
    return ESP_OK;
}

esp_err_t PwmController::setDuty(uint32_t dutyCycle) {
    if (!_isInitialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ledc_set_duty(_speedMode, CHANNEL, dutyCycle);
    if (err != ESP_OK) {
        return err;
    }

    return ledc_update_duty(_speedMode, CHANNEL);
}

esp_err_t PwmController::setPulseUs(uint32_t pulseUs) {
    if (!_isInitialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (pulseUs > _periodUs) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t duty = ((uint64_t)pulseUs * (_maxDutyCycle + 1)) / _periodUs;
    esp_err_t err = ledc_set_duty(_speedMode, CHANNEL, duty);
    if (err != ESP_OK) {
        return err;
    }

    return ledc_update_duty(_speedMode, CHANNEL);
}
