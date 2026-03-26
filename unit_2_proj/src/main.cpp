#include <Button.h>
#include <Led.h>

#include "LedController.h"
#include "MaintainedTrafficLight.h"
#include "TrafficLight.h"
#include "TrafficLightStateMachine.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

constexpr gpio_num_t LED_GREEN_PIN = GPIO_NUM_6;
constexpr gpio_num_t LED_YELLOW_PIN = GPIO_NUM_5;
constexpr gpio_num_t LED_RED_PIN = GPIO_NUM_4;

constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_0;

static void onTick(void* arg) {
    ITrafficLight* context = static_cast<ITrafficLight*>(arg);
    if (context == nullptr) {
        return;
    }

    context->onTick();
}

static void onButtonClick(void* arg) {
    TrafficLight* context = static_cast<TrafficLight*>(arg);
    if (context == nullptr) {
        return;
    }

    context->toggleMode();
}

extern "C" void app_main(void) {
    static Led greenLed(LED_GREEN_PIN);
    static Led yellowLed(LED_YELLOW_PIN);
    static Led redLed(LED_RED_PIN);

    static Button button = Button(BUTTON_PIN);

    static LedController greenController(greenLed);
    static LedController yellowController(yellowLed);
    static LedController redController(redLed);

    static TrafficLightStateMachine normal(greenController, yellowController, redController);
    static MaintainedTrafficLight maintained(greenController, yellowController, redController);

    static TrafficLight trafficLight(normal, maintained);

    ESP_ERROR_CHECK(greenLed.init(LedState::OFF));
    ESP_ERROR_CHECK(yellowLed.init(LedState::OFF));
    ESP_ERROR_CHECK(redLed.init(LedState::OFF));
    ESP_ERROR_CHECK(button.init());

    trafficLight.start();

    button.attachClick(onButtonClick, &trafficLight);

    esp_timer_handle_t timer;
    esp_timer_create_args_t timerArgs = {.callback = onTick, .arg = &trafficLight, .name = "trafficTick"};
    ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, ITrafficLight::TICK_PERIOD_MS * 1000));

    while (1) {
        button.tick();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
