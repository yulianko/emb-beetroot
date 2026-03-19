#include "PeriodicLogger.h"

#include "esp_log.h"

PeriodicLogger::PeriodicLogger(uint32_t intervalMs) : INTERVAL_US(intervalMs * 1000) {
}

void PeriodicLogger::tick() {
    int64_t now = esp_timer_get_time();
    if (now - _lastTickAt >= INTERVAL_US) {
        ESP_LOGI("LOGGER", ".");
        _lastTickAt = now;
    }
}
