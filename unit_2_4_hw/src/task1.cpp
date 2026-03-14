#include <Arduino.h>
#include "PeriodicLogger.h"

#ifdef PLATFORM_ESP32
#define BUTTON_PIN 4
#define ISR_ATTR IRAM_ATTR
#elif defined(PLATFORM_STM32)
#define BUTTON_PIN PA0
#define ISR_ATTR
#endif

constexpr uint8_t MAX_EVENTS = 20;
volatile uint32_t eventTimes[MAX_EVENTS];

volatile uint32_t counter = 0;
void ISR_ATTR buttonClickHandler()
{
    if (counter < MAX_EVENTS)
    {
        eventTimes[counter] = micros();
    }

    counter++;
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

    static uint32_t lastCounterReportValue = 0;

    uint32_t currentCounter = counter;
    if (currentCounter != lastCounterReportValue)
    {
        Serial.printf("\n\n[Task: 1] [Micros: %lu] Button pressed! Counter: %lu\n\n", micros(), currentCounter);
        if (currentCounter < MAX_EVENTS)
        {
            for (uint8_t i = currentCounter; i > lastCounterReportValue; i--)
            {
                Serial.printf("    [%d] %lu micros\n", i, eventTimes[i - 1]);
            }
        }

        lastCounterReportValue = currentCounter;
    }

    logger.tick();
}
