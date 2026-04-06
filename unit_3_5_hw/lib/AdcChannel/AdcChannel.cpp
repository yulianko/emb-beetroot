#include "AdcChannel.h"

AdcChannel::AdcChannel(AdcUnit& unit, adc_channel_t channel) : _unit(unit), _channel(channel) {
}

esp_err_t AdcChannel::init(adc_atten_t atten, adc_bitwidth_t bitwidth) {
    if (_isInitialized) {
        return ESP_ERR_INVALID_STATE;
    }

    adc_oneshot_chan_cfg_t config = {
        .atten = atten,
        .bitwidth = bitwidth,
    };

    adc_oneshot_unit_handle_t handle;
    esp_err_t err = _unit.getHandle(&handle);
    if (err != ESP_OK) {
        return err;
    }

    err = adc_oneshot_config_channel(handle, _channel, &config);
    if (err == ESP_OK) {
        _isInitialized = true;
    }

    return err;
}

esp_err_t AdcChannel::readRaw(int* raw) {
    if (!_isInitialized) {
        return ESP_ERR_INVALID_STATE;
    }

    adc_oneshot_unit_handle_t handle;
    esp_err_t err = _unit.getHandle(&handle);
    if (err != ESP_OK) {
        return err;
    }

    *raw = 0;
    return adc_oneshot_read(handle, _channel, raw);
}
