#include <stdio.h>
#include <stdlib.h>

#include <cmath>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

constexpr adc_unit_t ADC_UNIT = ADC_UNIT_1;
constexpr adc_channel_t ADC_CHANNEL = ADC_CHANNEL_3;
constexpr adc_atten_t ADC_ATTEN = ADC_ATTEN_DB_12;
constexpr adc_bitwidth_t ADC_BITWIDTH = ADC_BITWIDTH_12;

float getAdcMaxVoltage(adc_atten_t atten) {
    switch (atten) {
        case ADC_ATTEN_DB_0:
            return 950.0f;
        case ADC_ATTEN_DB_2_5:
            return 1250.0f;
        case ADC_ATTEN_DB_6:
            return 1750.0f;
        case ADC_ATTEN_DB_12:
            return 3100.0f;
        default:
            return 1100.0f;
    }
}

void configureOneShotAdcHandle(adc_oneshot_unit_handle_t* handle) {
    adc_oneshot_unit_init_cfg_t unitConfig = {
        .unit_id = ADC_UNIT,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfig, handle));

    adc_oneshot_chan_cfg_t channelConfig = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(*handle, ADC_CHANNEL, &channelConfig));
}

void configureCalibrationHandle(adc_cali_handle_t* handle) {
    adc_cali_curve_fitting_config_t calibrationConfig = {
        .unit_id = ADC_UNIT,
        .chan = ADC_CHANNEL,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };

    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&calibrationConfig, handle));
}

extern "C" void app_main(void) {
    adc_oneshot_unit_handle_t adcUnitHandle;
    configureOneShotAdcHandle(&adcUnitHandle);

    adc_cali_handle_t calibrationHandle = NULL;
    configureCalibrationHandle(&calibrationHandle);

    const float maxVoltage = getAdcMaxVoltage(ADC_ATTEN);

    printf("BITWIDTH ; ATTEN ; Max Voltage ; Raw ; U_manual(mV) ; U_cali(mV) ; Error(%%) ;\n");
    printf("-------- ; ----- ; ----------- ; --- ; ------------ ; ---------- ; --------- ;\n");

    float dMax = powf(2, ADC_BITWIDTH) - 1;
    while (1) {
        int adcRaw;
        adc_oneshot_read(adcUnitHandle, ADC_CHANNEL, &adcRaw);

        int calibratedVoltage;
        adc_cali_raw_to_voltage(calibrationHandle, adcRaw, &calibratedVoltage);

        float manualVoltage = (adcRaw * maxVoltage) / dMax;

        float error = 0;
        if (calibratedVoltage > 0) {
            error = (fabsf(manualVoltage - calibratedVoltage) / calibratedVoltage) * 100.0f;
        }

        printf("%d ; %d ; %.0f ; %d ; %.0f ; %d ; %.2f\n",
               ADC_BITWIDTH,
               ADC_ATTEN,
               maxVoltage,
               adcRaw,
               manualVoltage,
               calibratedVoltage,
               error);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
