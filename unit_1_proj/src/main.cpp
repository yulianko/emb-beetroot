#include <Arduino.h>
#include "OneButton.h"
#include "Led.h"

#define BUTTON_MODE 4
#define LED_MODE BUILTIN_LED

enum Mode
{
    FORCE_OFF,
    FORCE_ON,
    AUTO
};

Mode currentMode = FORCE_OFF;

OneButton modebutton(BUTTON_MODE, false, false);
Led modeIndicator(LED_MODE);

void nextMode()
{
    switch (currentMode)
    {
    case FORCE_OFF:
        currentMode = FORCE_ON;
        modeIndicator.setOn();
        break;
    case FORCE_ON:
        currentMode = AUTO;
        modeIndicator.setBlink();
        break;
    case AUTO:
        currentMode = FORCE_OFF;
        modeIndicator.setOff();
        break;
    }

    Serial.printf("Button Pressed. New mode is %d\n", currentMode);
}

void setup()
{
    Serial.begin(115200);
    modebutton.setDebounceMs(100);
    modebutton.attachClick(nextMode);
}

unsigned long lastPrint = 0;
void loop()
{
    if (millis() - lastPrint > 5000)
    {
        Serial.printf("5-sec loop report. Current mode is %d\n", currentMode);
        lastPrint = millis();
    }

    modebutton.tick();
    modeIndicator.tick();
}
