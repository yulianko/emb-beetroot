#pragma once

#include "I2CBus.h"
#include "esp_err.h"

typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bme280Calib;

typedef struct {
    i2cDevice* i2c;
    bme280Calib calib;
    int32_t t_fine;
} bme280;

typedef struct {
    float temperature;
    float humidity;
    float pressure;
} bme280Data;

esp_err_t bme280Init(bme280* dev, i2cDevice* i2c);
esp_err_t bme280Read(bme280* dev, bme280Data* data);
