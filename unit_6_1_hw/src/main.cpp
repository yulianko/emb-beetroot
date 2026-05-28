#include <esp_log.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static constexpr gpio_num_t LED_A_PIN = GPIO_NUM_4;
static constexpr gpio_num_t LED_B_PIN = GPIO_NUM_5;
static constexpr gpio_num_t LED_C_PIN = GPIO_NUM_6;

static const char* TAG_MAIN = "MAIN";

struct LedConfig {
    gpio_num_t pin;
    TickType_t period;
    const char* name;
};

static void ledTask(void* params) {
    if (params == nullptr) {
        ESP_LOGE("LED TASK", "invalid params is null, deleting task");
        vTaskDelete(nullptr);
        return;
    }

    const LedConfig* cfg = static_cast<const LedConfig*>(params);
    gpio_config_t io_conf = {.pin_bit_mask = (1ULL << cfg->pin),
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
    ESP_LOGI(cfg->name, "LED task started on pin %d with period %d ms", cfg->pin, cfg->period);
    while (1) {
        gpio_set_level(cfg->pin, 0);
        ESP_LOGI(cfg->name, "ON");
        vTaskDelay(cfg->period);
        gpio_set_level(cfg->pin, 1);
        ESP_LOGI(cfg->name, "OFF");
        vTaskDelay(cfg->period);
    }
}

extern "C" void app_main(void) {
    static const LedConfig ledA = {LED_A_PIN, pdMS_TO_TICKS(200), "LED A"};
    static const LedConfig ledB = {LED_B_PIN, pdMS_TO_TICKS(500), "LED B"};
    static const LedConfig ledC = {LED_C_PIN, pdMS_TO_TICKS(1000), "LED C"};

    ESP_LOGI(TAG_MAIN, "Starting LED tasks");
    xTaskCreate(ledTask, ledA.name, 2048, (void*)&ledA, 1, NULL);
    xTaskCreate(ledTask, ledB.name, 2048, (void*)&ledB, 1, NULL);
    xTaskCreate(ledTask, ledC.name, 2048, (void*)&ledC, 1, NULL);
}
