#include <Arduino.h>

constexpr uint8_t LED_PIN = LED_BUILTIN;
constexpr uint8_t BUTTON_PIN = 0;

class Config
{
public:
    static const uint32_t delayTime = 500;
    static const uint32_t baudRate = 115200;
    static const uint32_t logInterval = 1000;
};

enum class LedState
{
    On,
    Off
};

class Led
{
public:
    Led(uint8_t pin) : _pin(pin), _state(LedState::Off) {}

    void init(LedState initialState = LedState::Off)
    {
        pinMode(_pin, OUTPUT);
        set(initialState);
    }

    void set(LedState state)
    {
        if (_state == state)
        {
            return;
        }

        _state = state;
        digitalWrite(_pin, state == LedState::On ? HIGH : LOW);
    }

    void toggle()
    {
        set(_state == LedState::On ? LedState::Off : LedState::On);
    }

private:
    uint8_t _pin;
    LedState _state;
};

unsigned long previousBlink = 0;
Led led(LED_PIN);

volatile bool buttonPressed = false;
void onButtonPressed()
{
    buttonPressed = true;
}

enum class Mode
{
    ForceOn,
    ForceOff,
    Blinking
};

Mode mode = Mode::Blinking;

uint16_t counter = 0;

void setup()
{
    Serial.begin(Config::baudRate);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPressed, FALLING);

    led.init();
}

void loop()
{
    unsigned long currentTime = millis();

    if (buttonPressed)
    {
        buttonPressed = false;
        switch (mode)
        {
        case Mode::ForceOn:
        {
            mode = Mode::ForceOff;
            break;
        }
        case Mode::ForceOff:
        {
            mode = Mode::Blinking;
            previousBlink = 0;
            break;
        }
        case Mode::Blinking:
        {
            mode = Mode::ForceOn;
            break;
        }
        }
    }

    switch (mode)
    {
    case Mode::ForceOn:
    {
        led.set(LedState::On);
        break;
    }
    case Mode::ForceOff:
    {
        led.set(LedState::Off);
        break;
    }
    case Mode::Blinking:
    {
        if ((currentTime - previousBlink) >= Config::delayTime)
        {
            previousBlink = currentTime;
            led.toggle();
        }
        break;
    }
    }

    if (++counter >= Config::logInterval)
    {
        Serial.printf("Counter: %d, Loop duration: %lu\n", counter, millis() - currentTime);
        counter = 0;
    }
}
