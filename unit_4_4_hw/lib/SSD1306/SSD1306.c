#include "SSD1306.h"

#include <stdlib.h>
#include <string.h>

#include "SSD1306Font.h"

// #include "ssd1306_font.h"

// 0xAE        // Display OFF
// 0xD5 0x80   // Clock divide ratio
// 0xA8 0x3F   // Multiplex ratio (63 = 64 рядки)
// 0xD3 0x00   // Display offset
// 0x40        // Start line = 0
// 0x8D 0x14   // Charge pump ON (живлення від 3.3V без зовнішнього)
// 0x20 0x00   // Memory mode: Horizontal
// 0xA1        // Segment remap (дзеркало по X)
// 0xC8        // COM scan direction (дзеркало по Y)
// 0xDA 0x12   // COM pins config
// 0x81 0xCF   // Contrast
// 0xD9 0xF1   // Pre-charge period
// 0xDB 0x40   // VCOMH deselect
// 0xA4        // Display from RAM
// 0xA6        // Normal (не інвертований)
// 0xAF        // Display ON

#include <stdlib.h>
#include <string.h>

#include "SSD1306.h"
#include "SSD1306Font.h"

static esp_err_t sendCmd(ssd1306* display, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};  // 0x00 - Control byte
    return i2cWrite(display->i2c, buf, sizeof(buf));
}

static esp_err_t sendCmdWithArgs(ssd1306* display, uint8_t cmd, uint8_t arg) {
    uint8_t buf[3] = {0x00, cmd, arg};
    return i2cWrite(display->i2c, buf, sizeof(buf));
}

esp_err_t ssd1306Init(ssd1306* display, i2cDevice* i2c) {
    if (display == NULL || i2c == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    display->i2c = i2c;
    ssd1306Clear(display);

    sendCmd(display, 0xAE);                // Display OFF
    sendCmdWithArgs(display, 0xD5, 0x80);  // Clock divide ratio
    sendCmdWithArgs(display, 0xA8, 0x3F);  // Multiplex ratio
    sendCmdWithArgs(display, 0xD3, 0x00);  // Display offset
    sendCmd(display, 0x40);                // Start line
    sendCmdWithArgs(display, 0x8D, 0x14);  // Charge pump ON
    sendCmdWithArgs(display, 0x20, 0x00);  // Memory mode: Horizontal
    sendCmd(display, 0xA1);                // Segment remap
    sendCmd(display, 0xC8);                // COM scan direction
    sendCmdWithArgs(display, 0xDA, 0x12);  // COM pins config
    sendCmdWithArgs(display, 0x81, 0xCF);  // Contrast
    sendCmdWithArgs(display, 0xD9, 0xF1);  // Pre-charge period
    sendCmdWithArgs(display, 0xDB, 0x40);  // VCOMH deselect
    sendCmd(display, 0xA4);                // Display from RAM
    sendCmd(display, 0xA6);                // Normal
    return sendCmd(display, 0xAF);         // Display ON
}

esp_err_t ssd1306Clear(ssd1306* display) {
    if (display == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(display->frameBuffer, 0, SSD1306_FB_SIZE);
    return ESP_OK;
}

esp_err_t ssd1306Flush(ssd1306* display) {
    if (display == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t colCmd[4] = {0x00, 0x21, 0, SSD1306_WIDTH - 1};
    uint8_t pageCmd[4] = {0x00, 0x22, 0, SSD1306_PAGES - 1};

    i2cWrite(display->i2c, colCmd, sizeof(colCmd));
    i2cWrite(display->i2c, pageCmd, sizeof(pageCmd));

    uint8_t* buf = malloc(SSD1306_FB_SIZE + 1);
    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    buf[0] = 0x40;
    memcpy(buf + 1, display->frameBuffer, SSD1306_FB_SIZE);

    esp_err_t err = i2cWrite(display->i2c, buf, SSD1306_FB_SIZE + 1);
    free(buf);
    return err;
}

esp_err_t ssd1306DrawChar(ssd1306* display, uint8_t col, uint8_t page, char c) {
    if (display == NULL || col >= SSD1306_WIDTH || page >= SSD1306_PAGES) {
        return ESP_ERR_INVALID_ARG;
    }

    if (c < 32 || c > 126) {
        c = ' ';
    }

    const uint8_t* glyph = font5x7[c - 32];
    for (int i = 0; i < 5 && (col + i) < SSD1306_WIDTH; i++) {
        display->frameBuffer[page * SSD1306_WIDTH + col + i] = glyph[i];
    }

    if ((col + 5) < SSD1306_WIDTH) {
        display->frameBuffer[page * SSD1306_WIDTH + col + 5] = 0x00;
    }

    return ESP_OK;
}

esp_err_t ssd1306DrawString(ssd1306* display, uint8_t col, uint8_t page, const char* str) {
    if (display == NULL || str == NULL || col >= SSD1306_WIDTH || page >= SSD1306_PAGES) {
        return ESP_ERR_INVALID_ARG;
    }

    while (*str && col < SSD1306_WIDTH) {
        ssd1306DrawChar(display, col, page, *str++);
        col += 6;
    }

    return ESP_OK;
}
