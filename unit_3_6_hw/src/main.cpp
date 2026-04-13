#include <MelodyPlayer.h>
#include <PwmController.h>
#include <SafeStateMachine.h>
#include <Servo.h>
#include <stdio.h>

#include <memory>

#include "Button.h"
#include "Encoder.h"
#include "IQuadraturePcnt.h"
#include "Melodies/Melodies.h"
#include "QuadraturePcnt.h"
#include "SafeController/SafeController.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr int ENCODER_DEBOUNCE_NS = 1000;
static constexpr int ENCODER_MAX_COUNT = 1000;
static constexpr int ENCODER_MIN_COUNT = -1000;

static constexpr gpio_num_t ENCODER_BUTTON_PIN = GPIO_NUM_18;
static constexpr gpio_num_t ENCODER_A_INPUT_PIN = GPIO_NUM_17;
static constexpr gpio_num_t ENCODER_B_INPUT_PIN = GPIO_NUM_16;

static constexpr gpio_num_t BUZZER_PIN = GPIO_NUM_4;
static constexpr ledc_channel_t BUZZER_LEDC_CHANNEL = LEDC_CHANNEL_0;
static constexpr ledc_timer_t BUZZER_LEDC_TIMER = LEDC_TIMER_0;

static constexpr gpio_num_t SERVO_PIN = GPIO_NUM_5;
static constexpr ledc_channel_t SERVO_LEDC_CHANNEL = LEDC_CHANNEL_1;
static constexpr ledc_timer_t SERVO_LEDC_TIMER = LEDC_TIMER_1;

static constexpr uint8_t SECRET[] = {1, 7, 3};

static void handleState(SafeController::Command result,
                        SafeController& ctrl,
                        MelodyPlayer& medlodyPlayer,
                        Servo& servo);

extern "C" void app_main() {
    std::unique_ptr<IQuadraturePcnt> pcnt = std::make_unique<QuadraturePcnt>(ENCODER_A_INPUT_PIN, ENCODER_B_INPUT_PIN);
    Encoder2 encoder(std::move(pcnt));
    ESP_ERROR_CHECK(encoder.init(ENCODER_MAX_COUNT, ENCODER_MIN_COUNT, ENCODER_DEBOUNCE_NS));

    Button button(ENCODER_BUTTON_PIN);
    ESP_ERROR_CHECK(button.init());

    std::unique_ptr<PwmController> melodyPwm =
        std::make_unique<PwmController>(BUZZER_PIN, BUZZER_LEDC_CHANNEL, BUZZER_LEDC_TIMER);
    ESP_ERROR_CHECK(melodyPwm->init(MelodyPlayer::FREQUENCY_HZ, LEDC_TIMER_8_BIT));
    MelodyPlayer melodyPlayer(std::move(melodyPwm));

    std::unique_ptr<PwmController> servoPwm =
        std::make_unique<PwmController>(SERVO_PIN, SERVO_LEDC_CHANNEL, SERVO_LEDC_TIMER);
    servoPwm->init(Servo::FREQUENCY_HZ, LEDC_TIMER_12_BIT);
    Servo servo(std::move(servoPwm));

    std::unique_ptr<SafeStateMachine> safe = std::make_unique<SafeStateMachine>();
    SafeController controller(std::move(safe));

    servo.close();
    handleState(controller.begin(SECRET, sizeof(SECRET) / sizeof(SECRET[0]), 4), controller, melodyPlayer, servo);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100));

        if (controller.isEndOfWork()) {
            break;
        }

        Button::Event btn;
        ESP_ERROR_CHECK(button.tick(&btn));
        if (btn == Button::Event::CLICKED) {
            handleState(controller.onNewAttempt(), controller, melodyPlayer, servo);
            encoder.reset();
            continue;
        }

        Encoder2::State enc;
        ESP_ERROR_CHECK(encoder.tick(&enc));
        if (enc.accumulatedDelta == 0) {
            continue;
        }

        SafeController::Command result;
        if (enc.directionChanged) {
            result = controller.onDirectionChanged();
            encoder.reset();
        } else {
            result = controller.onIncrement(abs(enc.accumulatedDelta));
            encoder.consumeAccumulatedDelta();
        }

        handleState(result, controller, melodyPlayer, servo);
    }
}

static void handleState(SafeController::Command result,
                        SafeController& ctrl,
                        MelodyPlayer& melodyPlayer,
                        Servo& servo) {
    switch (result) {
        case SafeController::Command::NEW_ATTEMPT:
            melodyPlayer.play(NEW_ATTEMPT);
            printf("\n[A:%d] Code: 0", ctrl.attemptNumber() + 1);
            fflush(stdout);
            break;

        case SafeController::Command::NEW_ATTEMPT_AFTER_WRONG:
            melodyPlayer.play(WRONG_CODE);
            printf("\nWRONG CODE");
            melodyPlayer.play(NEW_ATTEMPT);
            printf("\n[A:%d] Code: 0", ctrl.attemptNumber() + 1);
            fflush(stdout);
            break;

        case SafeController::Command::DIGIT_UPDATED:
            printf("\b%d", ctrl.currentDigitValue());
            fflush(stdout);
            break;

        case SafeController::Command::DIGIT_CONFIRMED:
            printf("0");
            fflush(stdout);
            break;

        case SafeController::Command::GRANTED:
            melodyPlayer.play(GRANTED);
            printf("\nGRANTED\n");
            servo.open();
            break;

        case SafeController::Command::LOCKED:
            melodyPlayer.play(LOCKED);
            printf("\nLOCKED: Too many attempts\n");
            break;

        case SafeController::Command::NONE:
            break;
    }
}
