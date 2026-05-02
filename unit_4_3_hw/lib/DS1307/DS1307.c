#include "DS1307.h"

#include <string.h>

static esp_err_t writeBurst(ds1307* rtc, uint8_t startReg, uint8_t* data, size_t len) {
    uint8_t buf[len + 1];
    buf[0] = startReg;
    memcpy(&buf[1], data, len);
    return i2cWrite(rtc->i2c, buf, sizeof(buf));
}

static esp_err_t readBurst(ds1307* rtc, uint8_t startReg, uint8_t* buffer, size_t len) {
    esp_err_t ret = i2cWrite(rtc->i2c, &startReg, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    return i2cRead(rtc->i2c, buffer, len);
}

uint8_t dec2bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t bcd2dec(uint8_t val) {
    // Mask the high-order bits from noise
    return (((val & 0xF0) >> 4) * 10) + (val & 0x0F);
}

esp_err_t ds1307Init(ds1307* rtc, i2cDevice* i2c) {
    rtc->i2c = i2c;

    uint8_t reg = 0;
    esp_err_t ret = readBurst(rtc, 0x00, &reg, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    // Reset CH 7-bit
    reg &= 0x7F;

    uint8_t buf[2] = {0x00, reg};
    return i2cWrite(rtc->i2c, buf, sizeof(buf));
}

esp_err_t ds1307SetTime(ds1307* rtc, const struct tm* time) {
    uint8_t data[7] = {dec2bcd(time->tm_sec),
                       dec2bcd(time->tm_min),
                       dec2bcd(time->tm_hour),
                       dec2bcd(time->tm_wday + 1),
                       dec2bcd(time->tm_mday),
                       dec2bcd(time->tm_mon + 1),
                       dec2bcd(time->tm_year % 100)};

    return writeBurst(rtc, 0x00, data, 7);
}

esp_err_t ds1307GetTime(ds1307* rtc, struct tm* time) {
    uint8_t buf[7];

    esp_err_t ret = readBurst(rtc, 0x00, buf, 7);
    if (ret != ESP_OK) return ret;
    printf("RAW: %02X %02X %02X\n", buf[0], buf[1], buf[2]);

    time->tm_sec = bcd2dec(buf[0] & 0x7F);
    time->tm_min = bcd2dec(buf[1]);
    time->tm_hour = bcd2dec(buf[2] & 0x3F);
    time->tm_wday = bcd2dec(buf[3]) - 1;
    time->tm_mday = bcd2dec(buf[4]);
    time->tm_mon = bcd2dec(buf[5]) - 1;
    time->tm_year = bcd2dec(buf[6]) + 100;

    return ESP_OK;
}
