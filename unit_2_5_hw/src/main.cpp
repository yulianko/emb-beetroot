#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_err.h>

static const char *TAG = "UNIT 2.5";

constexpr gpio_num_t LED_PIN = GPIO_NUM_4;
constexpr gpio_num_t FAN_PIN = GPIO_NUM_5;

constexpr uint32_t TIMER_HZ = 1'000'000;

constexpr uint64_t TICKS_PERIOD = 7 * TIMER_HZ;
constexpr uint64_t TICKS_WORK = 4 * TIMER_HZ;
constexpr uint64_t TICKS_IDLE = TICKS_PERIOD - TICKS_WORK;

enum class State
{
    IDLE,
    WORKING
};

static gptimer_handle_t appTimer = nullptr;
static volatile State currentState = State::IDLE;

static volatile bool logStateOn = false;
static volatile bool logStateOff = false;

static bool IRAM_ATTR timerHandler(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata,
    void *user_ctx)
{
    gptimer_alarm_config_t alarmConfig = {};
    alarmConfig.flags.auto_reload_on_alarm = false;

    if (currentState == State::IDLE)
    {
        gpio_set_level(LED_PIN, 1);
        gpio_set_level(FAN_PIN, 1);

        currentState = State::WORKING;

        alarmConfig.alarm_count = edata->alarm_value + TICKS_WORK;

        logStateOn = true;
    }
    else
    {
        gpio_set_level(LED_PIN, 0);
        gpio_set_level(FAN_PIN, 0);

        currentState = State::IDLE;

        alarmConfig.alarm_count = edata->alarm_value + TICKS_IDLE;

        logStateOff = true;
    }

    gptimer_set_alarm_action(timer, &alarmConfig);

    return true;
}

extern "C" void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(FAN_PIN);
    gpio_set_direction(FAN_PIN, GPIO_MODE_OUTPUT);

    gptimer_config_t timerConfig =
        {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = TIMER_HZ};

    ESP_ERROR_CHECK(gptimer_new_timer(&timerConfig, &appTimer));

    gptimer_event_callbacks_t timerCallback = {.on_alarm = timerHandler};

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(appTimer, &timerCallback, nullptr));

    ESP_ERROR_CHECK(gptimer_enable(appTimer));

    gptimer_alarm_config_t alarmConfig = {
        .alarm_count = TICKS_PERIOD,
        .reload_count = 0,
        .flags = {.auto_reload_on_alarm = false}};

    ESP_ERROR_CHECK(gptimer_set_alarm_action(appTimer, &alarmConfig));
    ESP_ERROR_CHECK(gptimer_start(appTimer));

    while (1)
    {
        if (logStateOn)
        {
            logStateOn = false;
            ESP_LOGI(TAG, "System On");
        }

        if (logStateOff)
        {
            logStateOff = false;
            ESP_LOGI(TAG, "System Off");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
