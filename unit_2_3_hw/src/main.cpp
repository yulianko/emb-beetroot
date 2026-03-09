#include <Arduino.h>

constexpr uint8_t LED_1 = 35;
constexpr uint8_t LED_2 = 36;
constexpr uint8_t LED_3 = 37;

constexpr uint16_t PERIOD_LED_1 = 200;
constexpr uint16_t PERIOD_LED_2 = 500;
constexpr uint16_t PERIOD_LED_3 = 1000;

void setup()
{
    pinMode(LED_1, OUTPUT);
    digitalWrite(LED_1, LOW);

    pinMode(LED_2, OUTPUT);
    digitalWrite(LED_2, LOW);

    pinMode(LED_3, OUTPUT);
    digitalWrite(LED_3, LOW);
}

void loop()
{
    static bool stateLed1 = LOW;
    static bool stateLed2 = LOW;
    static bool stateLed3 = LOW;

    static unsigned long lastIntimeLed1 = 0;
    static unsigned long lastIntimeLed2 = 0;
    static unsigned long lastIntimeLed3 = 0;

    unsigned long currentTime = millis();
    if (currentTime - lastIntimeLed1 >= PERIOD_LED_1)
    {
        stateLed1 = !stateLed1;
        digitalWrite(LED_1, stateLed1);
        lastIntimeLed1 = currentTime;
    }

    if (currentTime - lastIntimeLed2 >= PERIOD_LED_2)
    {
        stateLed2 = !stateLed2;
        digitalWrite(LED_2, stateLed2);
        lastIntimeLed2 = currentTime;
    }

    if (currentTime - lastIntimeLed3 >= PERIOD_LED_3)
    {
        stateLed3 = !stateLed3;
        digitalWrite(LED_3, stateLed3);
        lastIntimeLed3 = currentTime;
    }
}
