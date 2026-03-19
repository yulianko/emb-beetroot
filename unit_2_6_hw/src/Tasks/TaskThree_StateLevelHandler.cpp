#include "TaskThree_StateLevelHandler.h"

#include "esp_log.h"

TaskThree_StateLevelHandler::TaskThree_StateLevelHandler(Led& led, ReadOnlyButton& button)
    : _led(led), _button(button) {
}

void TaskThree_StateLevelHandler::onInterrupt() {
    _pendingEvent = true;
}

void TaskThree_StateLevelHandler::tick() {
    if (!_pendingEvent.exchange(false)) {
        return;
    }

    if (_button.isPressedNow()) {
        _pressCounter++;
        ESP_LOGI(TAG, "Button Pressed: %d", _pressCounter.load());
        _led.toggle();
    }
}
