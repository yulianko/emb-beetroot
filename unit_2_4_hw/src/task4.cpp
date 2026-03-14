#include <Arduino.h>
#include "PeriodicLogger.h"

#define BUTTON_PIN PA0

constexpr unsigned long DEBOUNCE_MS = 50;
constexpr unsigned long POLL_INTERVAL_MS = 10;

enum class ButtonState
{
    IDLE,
    DEBOUNCING,
    PRESSED
};

void setup()
{
    Serial.begin(115200);

    // build-in pull-down
    pinMode(BUTTON_PIN, INPUT);
}

void loop()
{
    static PeriodicLogger logger(1000);

    static uint32_t counter = 0;
    static ButtonState state = ButtonState::IDLE;
    static unsigned long debounceStartAt = 0;

    static unsigned long lastPollAt = 0;

    unsigned long currentMillis = millis();

    if (currentMillis - lastPollAt >= POLL_INTERVAL_MS)
    {
        lastPollAt = currentMillis;
        bool isHigh = digitalRead(BUTTON_PIN) == HIGH;
        switch (state)
        {
        case ButtonState::IDLE:
        {
            if (isHigh)
            {
                state = ButtonState::DEBOUNCING;
                debounceStartAt = currentMillis;
            }

            break;
        }
        case ButtonState::DEBOUNCING:
        {
            if (!isHigh)
            {
                state = ButtonState::IDLE;
            }
            else
            {
                if (currentMillis - debounceStartAt >= DEBOUNCE_MS)
                {
                    state = ButtonState::PRESSED;
                    counter++;
                    Serial.printf("\n\n[Task: 4] [Millis: %lu] Button pressed! Counter: %lu\n\n", currentMillis, counter);
                }
            }

            break;
        }
        case ButtonState::PRESSED:
        {
            if (!isHigh)
            {
                state = ButtonState::IDLE;
            }

            break;
        }
        }
    }

    logger.tick();
}
