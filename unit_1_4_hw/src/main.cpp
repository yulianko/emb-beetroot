#include <Arduino.h>

const uint8_t LED_1 = LED_BUILTIN;
const uint8_t LED_2 = 16;

const uint8_t BTN_1 = 21;
const uint8_t BTN_2 = 0;

enum BlinkMode
{
    SLOW,
    FAST,
    TURBO
};

struct ModeConfig
{
    uint32_t onTime;
    uint32_t offTime;
};

enum ButtonState
{
    NONE_PRESSED,
    BTN_1_PRESSED,
    BTN_2_PRESSED,
    BOTH_PRESSED
};

class LedsController
{
    uint8_t led1;
    uint8_t led2;
    BlinkMode mode;
    const ModeConfig* modeConfigs;

    public:
        LedsController(
            uint8_t led1,
            uint8_t led2,
            BlinkMode mode,
            const ModeConfig* modeConfigs
        )
            : led1(led1), led2(led2), mode(mode), modeConfigs(modeConfigs)
        {}

        void begin()
        {
            pinMode(led1, OUTPUT);
            pinMode(led2, OUTPUT);
            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
        }

        void setMode(BlinkMode newMode)
        {
            Serial.print("Switching mode to: ");
            const char* modeName;
            switch (newMode)
            {
                case SLOW:
                {
                    modeName = "SLOW";
                    break;
                }
                case FAST:
                {
                    modeName = "FAST";
                    break;
                }
                case TURBO:
                {
                    modeName = "TURBO";
                    break;
                }
                default:
                {
                    modeName = "UNKNOWN";
                    break;
                }
            }

            Serial.println(modeName);

            mode = newMode;
        }

        void blinkDirect()
        {
            digitalWrite(led1, HIGH);
            digitalWrite(led2, LOW);
            delay(modeConfigs[mode].onTime);
        }

        void blinkReverse()
        {
            digitalWrite(led1, LOW);
            digitalWrite(led2, HIGH);
            delay(modeConfigs[mode].offTime);
        }
};


class ButtonsStateController
{
    uint8_t btn1;
    bool isPullUp1;
    uint8_t btn2;
    bool isPullUp2;

    public:
        ButtonsStateController(
            uint8_t btn1,
            bool isPullUp1,
            uint8_t btn2,
            bool isPullUp2)
            : btn1(btn1), btn2(btn2), isPullUp1(isPullUp1), isPullUp2(isPullUp2)
        {}

        void begin()
        {
            pinMode(btn1, isPullUp1 ? INPUT_PULLUP : INPUT);
            pinMode(btn2, isPullUp2 ? INPUT_PULLUP : INPUT);
        }

        ButtonState getState()
        {
            bool btn1State = digitalRead(btn1) == isPullUp1 ? LOW : HIGH;
            bool btn2State = digitalRead(btn2) == isPullUp2 ? LOW : HIGH;

            ButtonState state = NONE_PRESSED;
            if (btn1State)
            {
                if (!btn2State) {
                    Serial.println("Button BTN_1 pressed");
                    state = BTN_1_PRESSED;
                }
                else {
                    Serial.println("Button BTN_1 and BTN_2 pressed");
                    state = BOTH_PRESSED;
                }
            }
            else
            {
                if (btn2State) {
                    Serial.println("Button BTN_2 pressed");
                    state = BTN_2_PRESSED;
                }
            }

            if (state != NONE_PRESSED) {
                delay(1000);
            }

            return state;
        }
};

bool shouldSwitchMode(ButtonState state, BlinkMode& mode);

const ModeConfig MODE_CONFIGS[] =
{
    {600, 600},
    {200, 200},
    {50, 50}
};

ButtonsStateController buttonsStateController(BTN_1, false, BTN_2, true);

LedsController ledsController(LED_1, LED_2, SLOW, MODE_CONFIGS);

void setup()
{
    Serial.begin(115200);

    ledsController.begin();
    buttonsStateController.begin();
}

void loop()
{
    ButtonState state;
    BlinkMode mode;
    state = buttonsStateController.getState();
    if (shouldSwitchMode(state, mode))
    {
        ledsController.setMode(mode);
    }

    ledsController.blinkDirect();
    state = buttonsStateController.getState();
    if (shouldSwitchMode(state, mode))
    {
        ledsController.setMode(mode);
    }

    ledsController.blinkReverse();
}

bool shouldSwitchMode(ButtonState state, BlinkMode& mode)
{
    switch (state)
    {
        case BTN_1_PRESSED:
        {
            mode = FAST;
            return true;
        }
        case BTN_2_PRESSED:
        {
            mode = SLOW;
            return true;
        }
        case BOTH_PRESSED:
        {
            mode = TURBO;
            return true;
        }
        default: return false;
    }
}
