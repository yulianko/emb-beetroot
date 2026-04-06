#include <AdcChannel.h>
#include <AdcUnit.h>
#include <PwmController.h>
#include <SmaFilter.h>

#include <algorithm>

#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr gpio_num_t SERVO_PIN = GPIO_NUM_18;

static constexpr adc_unit_t ADC_UNIT = ADC_UNIT_1;
static constexpr adc_channel_t ADC_CHANNEL = ADC_CHANNEL_3;
static constexpr adc_atten_t ADC_ATTEN = ADC_ATTEN_DB_12;
static constexpr adc_bitwidth_t ADC_BITWIDTH = ADC_BITWIDTH_12;
static constexpr uint32_t ADC_MAX_VALUE = (1 << ADC_BITWIDTH) - 1;

static constexpr ledc_channel_t SERVO_LEDC_CHANNEL = LEDC_CHANNEL_0;
static constexpr ledc_timer_t SERVO_LEDC_TIMER = LEDC_TIMER_0;

// MG90s
static constexpr uint32_t SERVO_LEDC_FREQUENCY = 50;
static constexpr uint32_t SERVO_MIN_PULSE_US = 500;
static constexpr uint32_t SERVO_MAX_PULSE_US = 2400;
static constexpr uint32_t SERVO_MIN_ANGLE = 0;
static constexpr uint32_t SERVO_MAX_ANGLE = 180;

static constexpr uint32_t THREASHOLD = (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE);

uint32_t map(uint32_t x, uint32_t inMin, uint32_t inMax, uint32_t outMin, uint32_t outMax) {
    if (inMin == inMax) {
        return outMin;
    }

    x = std::clamp(x, inMin, inMax);

    return outMin + (uint64_t)(x - inMin) * (outMax - outMin) / (inMax - inMin);
}

extern "C" void app_main(void) {
    static AdcUnit adcUnit(ADC_UNIT);
    static AdcChannel potentiometer(adcUnit, ADC_CHANNEL);
    static PwmController servoPwmController(SERVO_PIN, SERVO_LEDC_CHANNEL, SERVO_LEDC_TIMER);

    ESP_ERROR_CHECK(adcUnit.init());
    ESP_ERROR_CHECK(potentiometer.init(ADC_ATTEN, ADC_BITWIDTH));
    ESP_ERROR_CHECK(servoPwmController.init(SERVO_LEDC_FREQUENCY));

    SmaFilter adcFilter(10);

    printf("Raw ADC;Filtered ADC;Servo Angle;Diff;Moved (%lu);\n", THREASHOLD);
    while (1) {
        static bool isMooved = false;
        static uint32_t lastPulseUs = 0;

        int adcRaw = 0;
        ESP_ERROR_CHECK(potentiometer.readRaw(&adcRaw));
        adcRaw = std::clamp(adcRaw, 0, (int)ADC_MAX_VALUE);

        adcFilter.addValue(adcRaw);
        const uint16_t adcFiltered = adcFilter.getAverage();

        const uint32_t pulseUs = map(adcFiltered, 0, ADC_MAX_VALUE, SERVO_MIN_PULSE_US, SERVO_MAX_PULSE_US);
        const uint32_t angle = map(adcFiltered, 0, ADC_MAX_VALUE, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

        if (std::abs((int32_t)pulseUs - (int32_t)lastPulseUs) > THREASHOLD) {
            lastPulseUs = pulseUs;
            ESP_ERROR_CHECK(servoPwmController.setPulseUs(pulseUs));
            isMooved = true;
        } else {
            isMooved = false;
        }

        printf("%d;%d;%ld;%ld;%d;\n", adcRaw, adcFiltered, angle, pulseUs - lastPulseUs, isMooved);

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
