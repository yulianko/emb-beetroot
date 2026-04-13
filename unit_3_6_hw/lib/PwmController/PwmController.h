#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "driver/ledc.h"
#include "esp_err.h"

class PwmController {
  public:
    PwmController(int pin, ledc_channel_t channel, ledc_timer_t timer, ledc_mode_t speedMode = LEDC_LOW_SPEED_MODE);

    esp_err_t init(uint32_t frequencyHz, ledc_timer_bit_t resolution, uint32_t initialDuty = 0);

    esp_err_t setDuty(uint32_t duty);
    uint32_t getMaxDutyCycle() const;
    uint32_t getFrequency() const;

  private:
    const int PIN;
    const ledc_channel_t CHANNEL;
    const ledc_timer_t TIMER;
    const ledc_mode_t SPEED_MODE;

    bool _initialized = false;

    uint32_t _maxDutyCycle = 0;
    uint32_t _frequencyHz = 0;
};

#endif
