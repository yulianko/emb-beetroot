#include <Led.h>
#include <PeriodicLogger.h>
#include <ReadOnlyButton.h>
#include <stdio.h>

#include <atomic>

#include "IInterruptable.h"
#include "Tasks/TaskFour_PollingDebounce.h"
#include "Tasks/TaskOne_RawInterruptHandler.h"
#include "Tasks/TaskThree_StateLevelHandler.h"
#include "Tasks/TaskTwo_DebounceHandler.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_21;

constexpr gpio_num_t LED_1_PIN = GPIO_NUM_1;
constexpr gpio_num_t LED_2_PIN = GPIO_NUM_2;
constexpr gpio_num_t LED_3_PIN = GPIO_NUM_4;
constexpr gpio_num_t LED_4_PIN = GPIO_NUM_16;

static void IRAM_ATTR gpioIsrHandler(void* arg) {
    IsrContext* ctx = static_cast<IsrContext*>(arg);

    for (size_t i = 0; i < ctx->size; i++) {
        ctx->observers[i]->onInterrupt();
    }
}

extern "C" void app_main(void) {
    ReadOnlyButton button(BUTTON_PIN);

    Led led1(LED_1_PIN);
    TaskOne_RawInterruptHandler task1(led1);

    Led led2(LED_2_PIN);
    TaskTwo_DebounceHandler task2(led2);

    Led led3(LED_3_PIN);
    TaskThree_StateLevelHandler task3(led3, button);

    Led led4(LED_4_PIN);
    TaskFour_PollingDebounce task4(led4, button);

    IInterruptable* observers[3] = {&task1, &task2, &task3};
    IsrContext ctx = {.observers = observers, .size = 3};

    button.init();
    led1.init();
    led2.init();
    led3.init();
    led4.init();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, gpioIsrHandler, &ctx);

    PeriodicLogger periodicLogger(2000);
    while (1) {
        task1.tick();
        task2.tick();
        task3.tick();
        task4.tick();
        periodicLogger.tick();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
