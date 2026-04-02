#include <SmaFilter.h>
#include <stdio.h>
#include <stdlib.h>

#include <cmath>

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr gpio_num_t LED_GPIO = GPIO_NUM_16;

static constexpr adc_unit_t ADC_UNIT = ADC_UNIT_1;
static constexpr adc_channel_t ADC_CHANNEL = ADC_CHANNEL_3;
static constexpr adc_atten_t ADC_ATTEN = ADC_ATTEN_DB_12;
static constexpr adc_bitwidth_t ADC_BITWIDTH = ADC_BITWIDTH_12;

static constexpr int LOW_VOLTAGE_THRESHOLD = 1700;
static constexpr int HIGH_VOLTAGE_THRESHOLD = 2000;

static constexpr int8_t SMA_FILTER_WINDOW_SIZE = 10;

void configureOneShotAdcHandle(adc_channel_t channel, adc_oneshot_unit_handle_t* handle) {
    adc_oneshot_unit_init_cfg_t unitConfig = {};
    unitConfig.unit_id = ADC_UNIT;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfig, handle));

    adc_oneshot_chan_cfg_t channelConfig = {};
    channelConfig.atten = ADC_ATTEN;
    channelConfig.bitwidth = ADC_BITWIDTH;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(*handle, channel, &channelConfig));
}

void configureLed(gpio_num_t led_gpio) {
    gpio_config_t config = {};
    config.pin_bit_mask = (1ULL << led_gpio);
    config.mode = GPIO_MODE_OUTPUT;
    gpio_config(&config);
}

extern "C" void app_main(void) {
    adc_oneshot_unit_handle_t adcUnitHandle;
    configureOneShotAdcHandle(ADC_CHANNEL, &adcUnitHandle);

    configureLed(LED_GPIO);
    int32_t ledState = 0;
    gpio_set_level(LED_GPIO, ledState);

    SmaFilter filter(SMA_FILTER_WINDOW_SIZE);

    printf("RAW; FILTERED %d; LEVEL;\n", SMA_FILTER_WINDOW_SIZE);
    while (1) {
        int adcRaw;
        adc_oneshot_read(adcUnitHandle, ADC_CHANNEL, &adcRaw);

        filter.addValue(adcRaw);
        float filtered = filter.getAverage();

        if (filtered > HIGH_VOLTAGE_THRESHOLD) {
            ledState = 0;
            gpio_set_level(LED_GPIO, ledState);

        } else {
            if (filtered < LOW_VOLTAGE_THRESHOLD) {
                ledState = 1;
                gpio_set_level(LED_GPIO, ledState);
            }
        }

        printf("%d; %.2f; %ld;\n", adcRaw, filtered, ledState);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
