#pragma once

#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"

typedef struct {
    spi_host_device_t host;
} spiBus;

typedef struct {
    spi_device_handle_t handle;
} spiDevice;

esp_err_t spiBusInit(
    spiBus* bus, spi_host_device_t host, gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk, int dmaChannel);

esp_err_t spiBusAddDevice(spiBus* bus, gpio_num_t cs, uint32_t clockHz, spiDevice* device);

esp_err_t spiTransfer(spiDevice* device, const uint8_t* txData, uint8_t* rxData, size_t len);
esp_err_t spiWrite(spiDevice* device, const uint8_t* data, size_t len);
esp_err_t spiRead(spiDevice* device, uint8_t reg, uint8_t* out, size_t len);
