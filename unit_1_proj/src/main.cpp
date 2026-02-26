#include <Arduino.h>
#include <OneButton.h>
#include "Led.h"
#include "LdrSensor.h"
#include "Actuators/LedActuator.h"

#define BUTTON_MODE 4
#define LED_MODE BUILTIN_LED
#define LED_ACTUATOR 16
#define LDR_SENSOR_PIN 6

enum Mode
{
    FORCE_OFF,
    FORCE_ON,
    AUTO
};

const char *modeName(Mode m)
{
    switch (m)
    {
    case FORCE_OFF:
        return "FORCE_OFF";
    case FORCE_ON:
        return "FORCE_ON";
    case AUTO:
        return "AUTO";
    }
    return "UNKNOWN";
}

Mode currentMode = FORCE_OFF;

OneButton modebutton(BUTTON_MODE, false, false);
Led modeIndicator(LED_MODE);

LdrSensor lightSensor(LDR_SENSOR_PIN);

LedActuator ledActuator(LED_ACTUATOR);
IActuator &actuator = ledActuator;

void nextMode()
{
    switch (currentMode)
    {
    case FORCE_OFF:
    {
        currentMode = FORCE_ON;
        modeIndicator.setOn();
        actuator.setOn();
        break;
    }
    case FORCE_ON:
    {
        currentMode = AUTO;
        modeIndicator.setBlink();
        bool isDark = lightSensor.isDark();
        if (isDark)
        {
            Serial.println("It's dark on button press. Turning on the actuator.");
            actuator.setOn();
        }
        else
        {
            Serial.println("It's bright on button press. Turning off the actuator.");
            actuator.setOff();
        }
        break;
    }
    case AUTO:
    {
        currentMode = FORCE_OFF;
        modeIndicator.setOff();
        actuator.setOff();
        break;
    }
    }

    Serial.printf("Button Pressed. New mode is %s\n", modeName(currentMode));
}

LdrSensorCallback handleSensorChange = [](bool isDark)
{
    if (currentMode != AUTO)
    {
        return;
    }

    if (isDark)
    {
        Serial.println("It's dark. Turning on the actuator.");
        actuator.setOn();
    }
    else
    {
        Serial.println("It's bright. Turning off the actuator.");
        actuator.setOff();
    }
};

void setup()
{
    Serial.begin(115200);

    modebutton.setDebounceMs(100);
    modebutton.attachClick(nextMode);

    lightSensor.setThreshold(1.8, 0.1);
    lightSensor.attachChange(handleSensorChange);
}

unsigned long lastPrint = 0;
void loop()
{
    if (millis() - lastPrint > 5000)
    {
        Serial.printf("5-sec loop report. Current mode is %s\n", modeName(currentMode));
        lastPrint = millis();
    }

    modebutton.tick();
    modeIndicator.tick();
    if (currentMode == AUTO)
    {
        lightSensor.tick();
    }
}
