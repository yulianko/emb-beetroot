#include "MelodyPlayer.h"

#include <math.h>

#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

MelodyPlayer::MelodyPlayer(std::unique_ptr<PwmController> pwm) : _pwm(std::move(pwm)) {
    initSine();
}

esp_err_t MelodyPlayer::play(const Melody& melody) {
    for (size_t i = 0; i < melody.size; ++i) {
        const MelodyNote& note = melody.notes[i];
        esp_err_t err = playNote(note.frequency, note.durationSec);
        if (err != ESP_OK) {
            return err;
        }
        err = stop();
        if (err != ESP_OK) {
            return err;
        }

        vTaskDelay(pdMS_TO_TICKS(PAUSE_DELAY_MS));
    }

    return ESP_OK;
}

esp_err_t MelodyPlayer::stop() {
    return _pwm->setDuty(0);
}

void MelodyPlayer::initSine() {
    for (size_t i = 0; i < SINE_SIZE; ++i) {
        _sineTable[i] = (uint32_t)((sin(2.0 * M_PI * i / SINE_SIZE) + 1.0) * 127.5);
    }
}

esp_err_t MelodyPlayer::playNote(uint16_t frequency, float durationSec) {
    if (frequency == 0) {
        vTaskDelay(pdMS_TO_TICKS((uint32_t)(durationSec * 1000)));
        return ESP_OK;
    }

    uint32_t delayUs = (uint32_t)(1'000'000.0 / frequency / SINE_SIZE);
    uint32_t repeats = (uint32_t)(durationSec * frequency);
    for (uint32_t r = 0; r < repeats; ++r) {
        esp_err_t err = playSine(delayUs);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t MelodyPlayer::playSine(uint32_t delayUs) {
    for (size_t i = 0; i < SINE_SIZE; ++i) {
        esp_err_t err = _pwm->setDuty(_sineTable[i]);
        if (err != ESP_OK) {
            return err;
        }

        esp_rom_delay_us(delayUs);
    }

    return ESP_OK;
}
