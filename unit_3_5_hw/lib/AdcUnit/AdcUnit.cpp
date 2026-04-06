#include "AdcUnit.h"

AdcUnit::AdcUnit(adc_unit_t unit_id) : UNIT_ID(unit_id) {
}

esp_err_t AdcUnit::init() {
    if (_isInitialized) {
        return ESP_ERR_INVALID_STATE;
    }

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = UNIT_ID,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,  // Default
        .ulp_mode = ADC_ULP_MODE_DISABLE,    // Default
    };

    esp_err_t err = adc_oneshot_new_unit(&init_config, &_handle);
    if (err != ESP_OK) {
        return err;
    }

    _isInitialized = true;

    return ESP_OK;
}

esp_err_t AdcUnit::getHandle(adc_oneshot_unit_handle_t* handle) const {
    if (!_isInitialized || !_handle) {
        return ESP_ERR_INVALID_STATE;
    }

    *handle = _handle;
    return ESP_OK;
}

AdcUnit::~AdcUnit() {
    if (_handle) {
        adc_oneshot_del_unit(_handle);
    }
}
