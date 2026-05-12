#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "I2cBus.h"
#include "esp_err.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_PAGES (SSD1306_HEIGHT / 8)
#define SSD1306_FB_SIZE (SSD1306_WIDTH * SSD1306_PAGES)

typedef struct {
    i2cDevice* i2c;
    uint8_t frameBuffer[SSD1306_FB_SIZE];
} ssd1306;

esp_err_t ssd1306Init(ssd1306* display, i2cDevice* device);
esp_err_t ssd1306Clear(ssd1306* display);
esp_err_t ssd1306Flush(ssd1306* display);

esp_err_t ssd1306DrawChar(ssd1306* display, uint8_t col, uint8_t page, char c);
esp_err_t ssd1306DrawString(ssd1306* display, uint8_t col, uint8_t page, const char* str);
