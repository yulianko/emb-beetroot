#include <esp_err.h>
#include <stdio.h>
#include <time.h>

#include "BME280.h"
#include "DS1307.h"
#include "I2cBus.h"
#include "SSD1306.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DISPLAY
#define RTC
#define BME

static const char* TAG = "main";

// I2C
static const i2c_port_num_t I2C_PORT_NUM = 0;
static const gpio_num_t I2C_SDA_PIN = GPIO_NUM_4;
static const gpio_num_t I2C_SCL_PIN = GPIO_NUM_5;

static const uint16_t SSD1306_ADDRESS = 0x3C;
static const uint16_t DS1307_ADDRESS = 0x68;
static const uint16_t BME280_ADDRESS = 0x76;

void printI2CBusScan(uint16_t address) {
    printf("I2C Device found: %#X", address);
    switch (address) {
        case SSD1306_ADDRESS:
            printf(" SSD1306 Display");
            break;
        case DS1307_ADDRESS:
            printf(" DS1307 RTC");
            break;
        case BME280_ADDRESS:
            printf(" BME280 Sensor");
            break;
        default:
            break;
    }
    printf("\n");
}

void app_main(void) {
    // I2C
    i2cBus bus;
    ESP_ERROR_CHECK(i2cBusInit(&bus, I2C_PORT_NUM, I2C_SDA_PIN, I2C_SCL_PIN));
    i2cBusScan(&bus, &printI2CBusScan);

    i2cDevice displayDevice;
    ssd1306 display;

    i2cDevice rtcDevice;
    ds1307 rtc;

    i2cDevice bmeDevice;
    bme280 bme;

#ifdef RTC
    ESP_ERROR_CHECK(i2cBusAddDevice(&bus, DS1307_ADDRESS, 10000, &rtcDevice));
    ESP_ERROR_CHECK(ds1307Init(&rtc, &rtcDevice));

#ifndef SET_START_TIME
    struct tm startTime = {
        .tm_sec = 0,
        .tm_min = 47,
        .tm_hour = 21,
        .tm_mday = 12,
        .tm_mon = 4,
        .tm_year = 2026 - 1900,
        .tm_wday = 2,
    };
    ds1307SetTime(&rtc, &startTime);
#endif
#endif

#ifdef DISPLAY
    ESP_ERROR_CHECK(i2cBusAddDevice(&bus, SSD1306_ADDRESS, 50000, &displayDevice));
    ESP_ERROR_CHECK(ssd1306Init(&display, &displayDevice));

    ESP_ERROR_CHECK(ssd1306Flush(&display));
#endif

#ifdef BME
    ESP_ERROR_CHECK(i2cBusAddDevice(&bus, BME280_ADDRESS, 50000, &bmeDevice));
    ESP_ERROR_CHECK(bme280Init(&bme, &bmeDevice));
#endif

    ESP_LOGI(TAG, "Initialized.");

    while (1) {
        bme280Data bmeData = {};

        struct tm timeinfo = {};
        char dateString[16];
        char timeString[9];

#ifdef RTC
        if (ds1307GetTime(&rtc, &timeinfo) != ESP_OK) {
            ESP_LOGE(TAG, "Could not get time");
        }
#endif

        strftime(dateString, sizeof(dateString), "%a %d.%m.%Y", &timeinfo);
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

#ifdef BME
        if (bme280Read(&bme, &bmeData) != ESP_OK) {
            ESP_LOGE(TAG, "Could not read BME280");
        }
#endif

        ESP_LOGI(TAG,
                 "%s %s | T: %.1f°C  RH: %.0f%%  P: %.1f hPa",
                 dateString,
                 timeString,
                 bmeData.temperature,
                 bmeData.humidity,
                 bmeData.pressure);

#ifdef DISPLAY
        char bmeTemp[16], bmeHum[10], bmePress[14];

        snprintf(bmeTemp, sizeof(bmeTemp), "T:  %.1f C", bmeData.temperature);
        snprintf(bmeHum, sizeof(bmeHum), "RH: %.0f%%", bmeData.humidity);
        snprintf(bmePress, sizeof(bmePress), "P: %.1f hPa", bmeData.pressure);

        ESP_ERROR_CHECK(ssd1306DrawString(&display, 10, 0, dateString));
        ESP_ERROR_CHECK(ssd1306DrawString(&display, 10, 1, timeString));
        ESP_ERROR_CHECK(ssd1306DrawString(&display, 10, 3, bmeTemp));
        ESP_ERROR_CHECK(ssd1306DrawString(&display, 10, 5, bmeHum));
        ESP_ERROR_CHECK(ssd1306DrawString(&display, 10, 7, bmePress));

        ESP_ERROR_CHECK(ssd1306Flush(&display));
#endif

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
