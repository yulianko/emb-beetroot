#include "SpiBus.h"

#include <string.h>

esp_err_t spiBusInit(
    spiBus* bus, spi_host_device_t host, gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk, int dmaChannel) {
    if (bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bus->host = host;

    spi_bus_config_t busConfig = {
        .mosi_io_num = mosi,
        .miso_io_num = miso,
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    return spi_bus_initialize(host, &busConfig, dmaChannel);
}

esp_err_t spiBusAddDevice(spiBus* bus, gpio_num_t cs, uint32_t clockHz, spiDevice* device) {
    if (bus == NULL || device == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_device_interface_config_t deviceConfig = {
        .clock_speed_hz = clockHz,
        .mode = 0,  // CPOL=0, CPHA=0
        .spics_io_num = cs,
        .queue_size = 1,
    };

    return spi_bus_add_device(bus->host, &deviceConfig, &device->handle);
}

esp_err_t spiTransfer(spiDevice* device, const uint8_t* txData, uint8_t* rxData, size_t len) {
    if (device == NULL || txData == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = txData,
        .rx_buffer = rxData,
    };

    return spi_device_transmit(device->handle, &t);
}

esp_err_t spiWrite(spiDevice* device, const uint8_t* data, size_t len) {
    if (device == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return spiTransfer(device, data, NULL, len);
}

esp_err_t spiRead(spiDevice* device, uint8_t reg, uint8_t* out, size_t len) {
    if (device == NULL || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t tx[len + 1];
    uint8_t rx[len + 1];

    tx[0] = reg | 0x80;
    for (size_t i = 1; i <= len; i++) tx[i] = 0x00;

    esp_err_t ret = spiTransfer(device, tx, rx, len + 1);
    if (ret == ESP_OK) {
        for (size_t i = 0; i < len; i++) out[i] = rx[i + 1];
    }

    return ret;
}
