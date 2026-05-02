#pragma once

#include <time.h>

#include "I2cBus.h"
#include "esp_err.h"

typedef struct {
    i2cDevice* i2c;
} ds1307;

esp_err_t ds1307Init(ds1307* rtc, i2cDevice* device);
esp_err_t ds1307SetTime(ds1307* rtc, const struct tm* time);
esp_err_t ds1307GetTime(ds1307* rtc, struct tm* time);
