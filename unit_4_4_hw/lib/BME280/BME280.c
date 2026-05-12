#include "BME280.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BME280_REG_ID 0xD0
#define BME280_REG_RESET 0xE0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_STATUS 0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_DATA 0xF7

#define BME280_REG_CALIB_00 0x88
#define BME280_REG_CALIB_26 0xE1

#define BME280_CHIP_ID 0x60
#define BME280_RESET_VALUE 0xB6

static esp_err_t writeReg(bme280* dev, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return i2cWrite(dev->i2c, buf, 2);
}

static esp_err_t readRegs(bme280* dev, uint8_t reg, uint8_t* out, size_t len) {
    esp_err_t ret = i2cWrite(dev->i2c, &reg, 1);
    if (ret != ESP_OK) return ret;
    return i2cRead(dev->i2c, out, len);
}

static esp_err_t readCalib(bme280* device) {
    uint8_t buf[26];

    esp_err_t ret = readRegs(device, BME280_REG_CALIB_00, buf, 24);
    if (ret != ESP_OK) {
        return ret;
    }

    bme280Calib* c = &device->calib;

    c->dig_T1 = (uint16_t)(buf[1] << 8 | buf[0]);
    c->dig_T2 = (int16_t)(buf[3] << 8 | buf[2]);
    c->dig_T3 = (int16_t)(buf[5] << 8 | buf[4]);

    c->dig_P1 = (uint16_t)(buf[7] << 8 | buf[6]);
    c->dig_P2 = (int16_t)(buf[9] << 8 | buf[8]);
    c->dig_P3 = (int16_t)(buf[11] << 8 | buf[10]);
    c->dig_P4 = (int16_t)(buf[13] << 8 | buf[12]);
    c->dig_P5 = (int16_t)(buf[15] << 8 | buf[14]);
    c->dig_P6 = (int16_t)(buf[17] << 8 | buf[16]);
    c->dig_P7 = (int16_t)(buf[19] << 8 | buf[18]);
    c->dig_P8 = (int16_t)(buf[21] << 8 | buf[20]);
    c->dig_P9 = (int16_t)(buf[23] << 8 | buf[22]);

    uint8_t h1;
    ret = readRegs(device, 0xA1, &h1, 1);
    if (ret != ESP_OK) return ret;
    c->dig_H1 = h1;

    uint8_t hbuf[7];
    ret = readRegs(device, BME280_REG_CALIB_26, hbuf, 7);
    if (ret != ESP_OK) return ret;

    c->dig_H2 = (int16_t)(hbuf[1] << 8 | hbuf[0]);
    c->dig_H3 = hbuf[2];
    c->dig_H4 = (int16_t)((hbuf[3] << 4) | (hbuf[4] & 0x0F));
    c->dig_H5 = (int16_t)((hbuf[5] << 4) | (hbuf[4] >> 4));
    c->dig_H6 = (int8_t)hbuf[6];

    return ESP_OK;
}

// https://github.com/boschsensortec/BME280_SensorAPI/blob/master/bme280.c
static int32_t compensateTemp(bme280* device, int32_t adcT) {
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((adcT / 8) - ((int32_t)device->calib.dig_T1 * 2));
    var1 = (var1 * ((int32_t)device->calib.dig_T2)) / 2048;
    var2 = (int32_t)((adcT / 16) - ((int32_t)device->calib.dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)device->calib.dig_T3)) / 16384;
    device->t_fine = var1 + var2;
    temperature = (device->t_fine * 5 + 128) / 256;

    if (temperature < temperature_min) {
        temperature = temperature_min;
    } else if (temperature > temperature_max) {
        temperature = temperature_max;
    }

    return temperature;
}

// https://github.com/boschsensortec/BME280_SensorAPI/blob/master/bme280.c
static uint32_t compensatePressure(bme280* device, int32_t adcP) {
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t pressure;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    var1 = (((int32_t)device->t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)device->calib.dig_P6);
    var2 = var2 + ((var1 * ((int32_t)device->calib.dig_P5)) * 2);
    var2 = (var2 / 4) + (((int32_t)device->calib.dig_P4) * 65536);
    var3 = (device->calib.dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)device->calib.dig_P2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)device->calib.dig_P1)) / 32768;

    /* Avoid exception caused by division by zero */
    if (var1) {
        var5 = (uint32_t)((uint32_t)1048576) - adcP;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;

        if (pressure < 0x80000000) {
            pressure = (pressure << 1) / ((uint32_t)var1);
        } else {
            pressure = (pressure / (uint32_t)var1) * 2;
        }

        var1 = (((int32_t)device->calib.dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)device->calib.dig_P8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + device->calib.dig_P7) / 16));

        if (pressure < pressure_min) {
            pressure = pressure_min;
        } else if (pressure > pressure_max) {
            pressure = pressure_max;
        }
    } else {
        pressure = pressure_min;
    }

    return pressure;
}

// https://github.com/boschsensortec/BME280_SensorAPI/blob/master/bme280.c
static double compensateHumidity(bme280* device, int32_t adcH) {
    double humidity;
    double humidity_min = 0.0;
    double humidity_max = 100.0;
    double var1;
    double var2;
    double var3;
    double var4;
    double var5;
    double var6;

    var1 = ((double)device->t_fine) - 76800.0;
    var2 = (((double)device->calib.dig_H4) * 64.0 + (((double)device->calib.dig_H5) / 16384.0) * var1);
    var3 = adcH - var2;
    var4 = ((double)device->calib.dig_H2) / 65536.0;
    var5 = (1.0 + (((double)device->calib.dig_H3) / 67108864.0) * var1);
    var6 = 1.0 + (((double)device->calib.dig_H6) / 67108864.0) * var1 * var5;
    var6 = var3 * var4 * (var5 * var6);
    humidity = var6 * (1.0 - ((double)device->calib.dig_H1) * var6 / 524288.0);

    if (humidity > humidity_max) {
        humidity = humidity_max;
    } else if (humidity < humidity_min) {
        humidity = humidity_min;
    }

    return humidity;
}

esp_err_t bme280Init(bme280* device, i2cDevice* i2c) {
    if (device == NULL || i2c == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    device->i2c = i2c;

    uint8_t id;
    esp_err_t ret = readRegs(device, BME280_REG_ID, &id, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    if (id != BME280_CHIP_ID) {
        return ESP_ERR_NOT_FOUND;
    }

    ret = writeReg(device, BME280_REG_RESET, BME280_RESET_VALUE);
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    ret = readCalib(device);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = writeReg(device, BME280_REG_CTRL_HUM, 0x01);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = writeReg(device, BME280_REG_CTRL_MEAS, 0x27);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = writeReg(device, BME280_REG_CONFIG, 0xA0);
    return ret;
}

esp_err_t bme280Read(bme280* device, bme280Data* data) {
    if (device == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t raw[8];

    esp_err_t ret = readRegs(device, BME280_REG_DATA, raw, 8);
    if (ret != ESP_OK) {
        return ret;
    }

    int32_t adcP = ((int32_t)raw[0] << 12) | ((int32_t)raw[1] << 4) | (raw[2] >> 4);
    int32_t adcT = ((int32_t)raw[3] << 12) | ((int32_t)raw[4] << 4) | (raw[5] >> 4);
    int32_t adcH = ((int32_t)raw[6] << 8) | (int32_t)raw[7];

    data->temperature = compensateTemp(device, adcT) / 100.0f;
    data->pressure = compensatePressure(device, adcP) / 100.0f;
    data->humidity = compensateHumidity(device, adcH);

    return ESP_OK;
}
