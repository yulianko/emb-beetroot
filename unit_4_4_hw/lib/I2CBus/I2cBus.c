#include "I2cBus.h"

esp_err_t i2cBusInit(i2cBus* bus, int port, int sda, int scl) {
    if (bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_config_t busConfig = {
        .i2c_port = port,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    return i2c_new_master_bus(&busConfig, &bus->handle);
}

void i2cBusScan(i2cBus* bus, i2cScanCallback callback) {
    if (bus == NULL) {
        return;
    }

    for (uint16_t a = 1; a < 127; a++) {
        if (i2c_master_probe(bus->handle, a, 100) == ESP_OK) {
            if (callback) {
                callback(a);
            }
        }
    }
}

esp_err_t i2cBusAddDevice(i2cBus* bus, uint16_t address, uint32_t freq, i2cDevice* device) {
    if (bus == NULL || device == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_device_config_t deviceConfig = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = freq,
    };

    return i2c_master_bus_add_device(bus->handle, &deviceConfig, &device->handle);
}

esp_err_t i2cWrite(i2cDevice* device, const uint8_t* data, size_t len) {
    if (device == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_transmit(device->handle, data, len, 100);
}

esp_err_t i2cRead(i2cDevice* device, uint8_t* out_data, size_t len) {
    if (device == NULL || out_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_receive(device->handle, out_data, len, 100);
}
