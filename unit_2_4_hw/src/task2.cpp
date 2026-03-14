#include <Arduino.h>
#include "PeriodicLogger.h"

#define BUTTON_PIN PA0

constexpr unsigned long DEBOUNCE_MS = 50;

volatile bool isButtonRising = false;
void buttonClickHandler()
{
    isButtonRising = true;
}

void setup()
{
    Serial.begin(115200);

    // build-in pull-down
    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonClickHandler, RISING);
}

void loop()
{
    static PeriodicLogger logger(1000);

    static uint32_t counter = 0;
    static unsigned long lastCountedAt = 0;

    unsigned long currentMillis = millis();

    if (isButtonRising)
    {
        if (currentMillis - lastCountedAt >= DEBOUNCE_MS)
        {
            lastCountedAt = currentMillis;
            isButtonRising = false;
            counter++;
            Serial.printf("\n\n[Task: 2] [Millis: %lu] Button pressed! Counter: %lu\n\n", currentMillis, counter);
        }
    }

    logger.tick();
}
