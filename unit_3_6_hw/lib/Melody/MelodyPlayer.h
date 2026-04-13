#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#pragma once
#include <memory>

#include "Melody.h"
#include "PwmController.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

class MelodyPlayer {
  public:
    static const uint16_t FREQUENCY_HZ = 18000;

    MelodyPlayer(std::unique_ptr<PwmController> pwm);

    esp_err_t play(const Melody& melody);
    esp_err_t stop();

  private:
    std::unique_ptr<PwmController> _pwm;

    static constexpr uint32_t PAUSE_DELAY_MS = 50;
    static constexpr size_t SINE_SIZE = 32;
    uint32_t _sineTable[SINE_SIZE];

    void initSine();
    esp_err_t playSine(uint32_t delayUs);
    esp_err_t playNote(uint16_t frequency, float durationSec);
};

#endif
