#pragma once
#include "driver/i2c_master.h"

typedef struct {
    i2c_master_bus_handle_t handle;
} i2cBus;

typedef struct {
    i2c_master_dev_handle_t handle;
} i2cDevice;

typedef void (*i2cScanCallback)(uint16_t address);

esp_err_t i2cBusInit(i2cBus* bus, int port, int sda, int scl);

void i2cBusScan(i2cBus* bus, i2cScanCallback callback);

esp_err_t i2cBusAddDevice(i2cBus* bus, uint16_t address, uint32_t freq, i2cDevice* device);

esp_err_t i2cWrite(i2cDevice* device, const uint8_t* data, size_t len);
esp_err_t i2cRead(i2cDevice* device, uint8_t* out_data, size_t len);
