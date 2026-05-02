#include <esp_err.h>
#include <stdio.h>
#include <time.h>

#include "DS1307.h"
#include "I2cBus.h"
#include "SSD1306.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DISPLAY
#define RTC

static const i2c_port_num_t I2C_PORT_NUM = 0;
static const gpio_num_t I2C_SDA_PIN = GPIO_NUM_4;
static const gpio_num_t I2C_SCL_PIN = GPIO_NUM_5;

static const uint16_t SSD1306_ADDRESS = 0x3C;
static const uint16_t DS1307_ADDRESS = 0x68;

void printBusScan(uint16_t address) {
    printf("Device found: %#X", address);
    switch (address) {
        case SSD1306_ADDRESS:
            printf(" SSD1306 Display");
            break;
        case DS1307_ADDRESS:
            printf(" DS1307 RTC");
            break;
        default:
            break;
    }

    printf("\n");
}

void app_main(void) {
    i2cBus bus;

    i2cDevice displayDevice;
    ssd1306 display;

    i2cDevice rtcDevice;
    ds1307 rtc;

    ESP_ERROR_CHECK(i2cBusInit(&bus, I2C_PORT_NUM, I2C_SDA_PIN, I2C_SCL_PIN));
    i2cBusScan(&bus, &printBusScan);

#ifdef RTC
    printf("DS1307 RTC Initialization");
    ESP_ERROR_CHECK(i2cBusAddDevice(&bus, DS1307_ADDRESS, 10000, &rtcDevice));
    ESP_ERROR_CHECK(ds1307Init(&rtc, &rtcDevice));

    struct tm startTime = {
        .tm_sec = 0, .tm_min = 30, .tm_hour = 12, .tm_mday = 8, .tm_mon = 4, .tm_year = 2026 - 1900, .tm_wday = 5};
    // ds1307SetTime(&rtc, &startTime);
#endif

#ifdef DISPLAY
    printf("SSD1306 Display Initialization");
    ESP_ERROR_CHECK(i2cBusAddDevice(&bus, SSD1306_ADDRESS, 50000, &displayDevice));
    ESP_ERROR_CHECK(ssd1306Init(&display, &displayDevice));

    ssd1306DrawString(&display, 10, 2, "SSD1306 Display");
    ESP_ERROR_CHECK(ssd1306Flush(&display));
#endif

    printf("Initialized.\n");

    while (1) {
        struct tm timeinfo = {};

#ifdef RTC
        if (ds1307GetTime(&rtc, &timeinfo) != ESP_OK) {
            printf("Could not get time\n");
        } else {
            printf("RTC: ");
        }
#endif

        char dateString[16];
        char timeString[9];

        strftime(dateString, sizeof(dateString), "%a %d.%m.%Y", &timeinfo);
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

        printf("%s %s\n", dateString, timeString);

#ifdef DISPLAY
        ssd1306DrawString(&display, 10, 4, dateString);
        ssd1306DrawString(&display, 10, 6, timeString);
        ssd1306Flush(&display);
#endif
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
