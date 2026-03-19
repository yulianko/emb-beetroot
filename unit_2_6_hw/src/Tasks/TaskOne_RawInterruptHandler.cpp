#include "TaskOne_RawInterruptHandler.h"

#include "esp_log.h"

TaskOne_RawInterruptHandler::TaskOne_RawInterruptHandler(Led& led) : _led(led) {
}

void TaskOne_RawInterruptHandler::onInterrupt() {
    _interruptsCounter++;
    _interruptHandled = false;
}

void TaskOne_RawInterruptHandler::tick() {
    if (!_interruptHandled.exchange(true)) {
        ESP_LOGI(TAG, "Button Pressed: %d", _interruptsCounter.load());

        _led.toggle();
    }
}
