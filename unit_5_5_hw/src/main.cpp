#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <algorithm>
#include <cmath>

#include "Drv8833Module.h"
#include "QuadraturePcnt.h"

// #define STOP_ON_TARGET_ANGLE

static constexpr const char* TAG = "Main";

static constexpr gpio_num_t PIN_STBY = GPIO_NUM_8;
static constexpr gpio_num_t PIN_AIN1 = GPIO_NUM_9;
static constexpr gpio_num_t PIN_AIN2 = GPIO_NUM_10;
static constexpr int MCPWM_GROUP = 0;

static constexpr gpio_num_t PIN_ENCODER_CLK = GPIO_NUM_4;
static constexpr gpio_num_t PIN_ENCODER_DT = GPIO_NUM_5;
static constexpr int16_t COUNTS_PER_REV = 80;

static constexpr float MOTOR_SUPPLY_POWER = 4.8f;
static constexpr float MOTOR_MIN_POWER = 4.0f;
static constexpr float MOTOR_SPEED_THRESHOLD = MOTOR_MIN_POWER / MOTOR_SUPPLY_POWER;

static constexpr float SET_POINT = 90.0f;
static constexpr float Kp = 0.8f;
static constexpr float Ki = 0.01f;
static constexpr float Kd = 0.9f;

static constexpr float errorThreshold = 5.0f;

float getAngle(int16_t raw) {
    return (static_cast<float>(raw) / COUNTS_PER_REV) * 360.0f;
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "---- Starting app main ----");

    DRV8833Module motor({
        .pinAIn1 = PIN_AIN1,
        .pinAIn2 = PIN_AIN2,
        .pinStby = PIN_STBY,
        .mcpwmGroupId = MCPWM_GROUP,
        .pwmFreqHz = 20'000,
    });
    ESP_ERROR_CHECK(motor.init());

    QuadraturePcnt encoder(PIN_ENCODER_CLK, PIN_ENCODER_DT);
    ESP_ERROR_CHECK(encoder.init(100, -100, 100));

    ESP_LOGI(TAG, "---- Initialized ----");

    float integral = 0, lastError = 0;
    float derivative = 0;

    int64_t lastTime = esp_timer_get_time();
    printf("Time; Position; Angle; Error; Output; Speed\n");
    while (1) {
        int64_t now = esp_timer_get_time();
        float actualDt = (now - lastTime) / 1'000'000.0f;
        lastTime = now;

        int16_t position = 0;
        QuadraturePcnt::Signal clkState{};
        QuadraturePcnt::Signal dtState{};
        ESP_ERROR_CHECK(encoder.tick(&position, &clkState, &dtState));
        float currentAngle = getAngle(position);

        float error = SET_POINT - currentAngle;

        if (std::fabs(error) <= errorThreshold) {
#ifdef STOP_ON_TARGET_ANGLE
            motor.brakeA();
            ESP_LOGI(TAG, "Position: %d Angle: %f  Error: %f  Output: 0 Speed: 0", position, currentAngle, error);
            ESP_LOGI(TAG,
                     "Error %f is less than %f threshold, braking motor",
                     error,
                     errorThreshold,
                     position,
                     currentAngle);
            break;
#else
            continue;
#endif
        }

        integral += error * actualDt;
        derivative = (error - lastError) / actualDt;

        float output = Kp * error + Ki * integral + Kd * derivative;

        output = std::clamp(output, -100.0f, 100.0f);

        float speed = output / 100.0f * (1 - MOTOR_SPEED_THRESHOLD);
        if (output > 0) {
            speed += MOTOR_SPEED_THRESHOLD;
        } else if (output < 0) {
            speed -= MOTOR_SPEED_THRESHOLD;
        }

        printf("%lld;%d;%f;%f;%f;%f\n", now, position, currentAngle, error, output, speed);

        motor.setSpeedA(speed);
        lastError = error;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
