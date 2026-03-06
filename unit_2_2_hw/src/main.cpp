#include <Arduino.h>

constexpr uint8_t BUTTON = 0;
constexpr uint8_t RELAY = 16;
constexpr uint8_t IN_SIGNAL = 4;

volatile bool buttonPressed = false;
void IRAM_ATTR handleButtonPress()
{
    buttonPressed = true;
}

bool relayState = false;
volatile bool signalReceived = false;
void IRAM_ATTR handleInputSignal()
{
    signalReceived = true;
}

void setup()
{
    Serial.begin(115200);
    pinMode(BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON), handleButtonPress, FALLING);

    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, LOW);

    pinMode(IN_SIGNAL, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(IN_SIGNAL), handleInputSignal, CHANGE);
}

long counter = 0;
unsigned long timeSent = 0;
unsigned long elapsedTime = 0;
unsigned long totalElapsedTime = 0;

void loop()
{
    if (buttonPressed)
    {
        counter++;
        totalElapsedTime += elapsedTime;
        Serial.println("Button Pressed! Counter: " + String(counter));
        buttonPressed = false;
        relayState = !relayState;
        timeSent = millis();
        digitalWrite(RELAY, relayState ? HIGH : LOW);
        Serial.println(relayState ? "Relay ON" : "Relay OFF");
        if (counter > 1)
        {
            Serial.println("Avg wait time for input signal: " + String(totalElapsedTime / (counter - 1)) + " ms");
        }
    }

    if (signalReceived)
    {
        signalReceived = false;
        elapsedTime = millis() - timeSent;
        Serial.println("Input Signal: " + String(digitalRead(IN_SIGNAL) == HIGH ? "HIGH" : "LOW") + " at " + String(elapsedTime) + " ms since last button press");
    }
}
