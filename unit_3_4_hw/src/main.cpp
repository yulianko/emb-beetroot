#include <esp_task_wdt.h>
#include <math.h>

#include "driver/ledc.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const gpio_num_t PWM_PIN = GPIO_NUM_5;

static const gpio_num_t BTN_1 = GPIO_NUM_16;
static const gpio_num_t BTN_2 = GPIO_NUM_17;
static const gpio_num_t BTN_3 = GPIO_NUM_18;

static constexpr double C4 = 262.0;
static constexpr double D4 = 294.0;
static constexpr double E4 = 330.0;
static constexpr double F4 = 349.0;
static constexpr double G4 = 392.0;
static constexpr double A4 = 440.0;
static constexpr double B4 = 494.0;
static constexpr double C5 = 523.0;

static constexpr double BTN_1_NOTE_1 = E4;
static constexpr double BTN_2_NOTE_2 = G4;
static constexpr double BTN_3_NOTE_3 = C4;

static constexpr double SETUP_NOTES[] = {C4, D4, E4, F4, G4, A4, B4, C5};
static constexpr size_t SETUP_NOTES_SIZE = sizeof(SETUP_NOTES) / sizeof(SETUP_NOTES[0]);

static constexpr size_t SINE_SIZE = 32;
uint32_t sineTable[SINE_SIZE];

void initSine() {
    for (int i = 0; i < SINE_SIZE; i++) {
        sineTable[i] = (uint32_t)((sin(2.0 * M_PI * i / SINE_SIZE) + 1.0) * 127.5);
    }
}

void playSine(uint32_t delayUs) {
    for (int i = 0; i < SINE_SIZE; i++) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, sineTable[i]);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        esp_rom_delay_us(delayUs);
    }
}

void stopPlaying() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void playNote(double noteFreq, double durationSec) {
    double delayUs = (1.0 / noteFreq * 1'000'000.0) / SINE_SIZE;
    int repeatCount = (int)(durationSec * noteFreq);
    for (int r = 0; r < repeatCount; r++) {
        playSine((uint32_t)delayUs);
    }
}

void playNoteWhilePressed(double noteFreq, gpio_num_t btnPin) {
    double delayUs = (1.0 / noteFreq * 1'000'000.0) / SINE_SIZE;
    while (gpio_get_level(btnPin) == 1) {
        esp_task_wdt_reset();
        playSine((uint32_t)delayUs);
    }

    stopPlaying();
}

extern "C" void app_main(void) {
    esp_task_wdt_add(NULL);

    initSine();

    ledc_timer_config_t timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                 .duty_resolution = LEDC_TIMER_8_BIT,
                                 .timer_num = LEDC_TIMER_0,
                                 .freq_hz = 18000,
                                 .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer);

    ledc_channel_config_t chan = {.gpio_num = PWM_PIN,
                                  .speed_mode = LEDC_LOW_SPEED_MODE,
                                  .channel = LEDC_CHANNEL_0,
                                  .timer_sel = LEDC_TIMER_0,
                                  .duty = 0};
    ledc_channel_config(&chan);

    gpio_config_t btnConfig = {.pin_bit_mask = (1ULL << BTN_1) | (1ULL << BTN_2) | (1ULL << BTN_3),
                               .mode = GPIO_MODE_INPUT,
                               .pull_up_en = GPIO_PULLUP_DISABLE,
                               .pull_down_en = GPIO_PULLDOWN_ENABLE,
                               .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&btnConfig);

    for (size_t j = 0; j < SETUP_NOTES_SIZE; j++) {
        playNote(SETUP_NOTES[j], 0.5);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    while (1) {
        if (gpio_get_level(BTN_1) == 1) {
            playNoteWhilePressed(BTN_1_NOTE_1, BTN_1);
        } else if (gpio_get_level(BTN_2) == 1) {
            playNoteWhilePressed(BTN_2_NOTE_2, BTN_2);
        } else if (gpio_get_level(BTN_3) == 1) {
            playNoteWhilePressed(BTN_3_NOTE_3, BTN_3);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
        esp_task_wdt_reset();
    }
}
