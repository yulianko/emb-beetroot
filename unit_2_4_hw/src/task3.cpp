#include <Arduino.h>
#include "PeriodicLogger.h"

#define BUTTON_PIN PA0

volatile bool isButtonStateChanged = false;
void buttonClickHandler()
{
    isButtonStateChanged = true;
}

void setup()
{
    Serial.begin(115200);

    // build-in pull-down
    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonClickHandler, CHANGE);
}

void loop()
{
    static PeriodicLogger logger(1000);

    static uint32_t counter = 0;

    unsigned long currentMillis = millis();

    if (isButtonStateChanged)
    {
        isButtonStateChanged = false;
        if (digitalRead(BUTTON_PIN) == HIGH)
        {
            counter++;
            Serial.printf("\n\n[Task: 3] [Millis: %lu] Button pressed! Counter: %lu\n\n", currentMillis, counter);
        }
    }

    logger.tick();
}
