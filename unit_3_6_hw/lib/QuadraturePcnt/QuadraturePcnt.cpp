#include "QuadraturePcnt.h"

QuadraturePcnt::QuadraturePcnt(gpio_num_t clk, gpio_num_t dt) : PIN_CLK(clk), PIN_DT(dt) {
}

esp_err_t QuadraturePcnt::init(int16_t maxValue, int16_t minValue, uint16_t debounceNs) {
    pcnt_unit_config_t unitConfig = {.low_limit = minValue,
                                     .high_limit = maxValue,
                                     .intr_priority = 0,
                                     .flags = {
                                         .accum_count = 0,
                                     }};
    esp_err_t err;
    _unit = NULL;
    err = pcnt_new_unit(&unitConfig, &_unit);
    if (err != ESP_OK) {
        return err;
    }

    pcnt_glitch_filter_config_t filterConfig = {.max_glitch_ns = debounceNs};
    err = pcnt_unit_set_glitch_filter(_unit, &filterConfig);
    if (err != ESP_OK) {
        return err;
    }

    pcnt_chan_config_t chanConfigA = {
        .edge_gpio_num = PIN_CLK,
        .level_gpio_num = PIN_DT,
        .flags =
            {
                .invert_edge_input = 0,
                .invert_level_input = 0,
                .virt_edge_io_level = 0,
                .virt_level_io_level = 0,
                .io_loop_back = 0,
            },
    };
    pcnt_channel_handle_t chanA = NULL;
    err = pcnt_new_channel(_unit, &chanConfigA, &chanA);
    if (err != ESP_OK) {
        return err;
    }

    pcnt_chan_config_t chanConfigB = {
        .edge_gpio_num = PIN_DT,
        .level_gpio_num = PIN_CLK,
        .flags =
            {
                .invert_edge_input = 0,
                .invert_level_input = 0,
                .virt_edge_io_level = 0,
                .virt_level_io_level = 0,
                .io_loop_back = 0,
            },
    };
    pcnt_channel_handle_t chanB = NULL;
    err = pcnt_new_channel(_unit, &chanConfigB, &chanB);
    if (err != ESP_OK) {
        return err;
    }

    err = pcnt_channel_set_edge_action(chanA, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    if (err != ESP_OK) {
        return err;
    }
    err = pcnt_channel_set_level_action(chanA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    if (err != ESP_OK) {
        return err;
    }
    err = pcnt_channel_set_edge_action(chanB, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
    if (err != ESP_OK) {
        return err;
    }
    err = pcnt_channel_set_level_action(chanB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    if (err != ESP_OK) {
        return err;
    }

    err = pcnt_unit_enable(_unit);
    if (err != ESP_OK) {
        return err;
    }
    err = pcnt_unit_clear_count(_unit);
    if (err != ESP_OK) {
        return err;
    }
    err = pcnt_unit_start(_unit);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}

esp_err_t QuadraturePcnt::tick(int16_t* position, Signal* clkState, Signal* dtState) {
    int count;
    esp_err_t err = pcnt_unit_get_count(_unit, &count);
    if (err != ESP_OK) {
        return err;
    }

    *position = count;
    *clkState = gpio_get_level(PIN_CLK) ? HIGH : LOW;
    *dtState = gpio_get_level(PIN_DT) ? HIGH : LOW;

    return ESP_OK;
}

esp_err_t QuadraturePcnt::reset() {
    return pcnt_unit_clear_count(_unit);
}
