#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "driver/ledc.h"

class PwmController {
  public:
    PwmController(int pin, ledc_channel_t channel, ledc_timer_t timer);

    esp_err_t init(uint32_t frequency,
                   uint32_t dutyCycle = 0,
                   ledc_timer_bit_t dutyResolution = LEDC_TIMER_10_BIT,
                   ledc_mode_t speedMode = LEDC_LOW_SPEED_MODE);

    esp_err_t setDuty(uint32_t dutyCycle);

    esp_err_t setPulseUs(uint32_t pulseUs);

  private:
    bool _isInitialized = false;

    const int PIN;
    const ledc_channel_t CHANNEL;
    const ledc_timer_t TIMER;

    uint32_t _periodUs{};
    ledc_mode_t _speedMode{};
    uint32_t _maxDutyCycle{};
};

#endif
