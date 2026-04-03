#include <Arduino.h>
#include <Math.h>
#include <SmaFilter.h>
#include <driver/ledc.h>

static constexpr uint8_t POT_PIN = 4;
static constexpr uint8_t LED_PWM_PIN = 16;
static constexpr uint8_t MOTOR_PWM_PIN = 18;

static constexpr uint8_t ANALOG_RESOLUTION = 12;
static constexpr uint16_t ANALOG_MAX_VALUE = (1U << ANALOG_RESOLUTION) - 1;
static constexpr int ADC_THRESHOLD = 4;
static constexpr int ADC_SMA_WINDOW = 6;

static constexpr ledc_channel_t LED_LEDC_CHANNEL = LEDC_CHANNEL_0;
static constexpr ledc_timer_t LED_LEDC_TIMER = LEDC_TIMER_0;
static constexpr uint32_t LED_LEDC_FREQUENCY = 1000;

static constexpr ledc_channel_t MOTOR_LEDC_CHANNEL = LEDC_CHANNEL_1;
static constexpr ledc_timer_t MOTOR_LEDC_TIMER = LEDC_TIMER_1;
static constexpr uint32_t MOTOR_LEDC_FREQUENCY = 10000;

class PwmController {
  public:
    PwmController(int pin, ledc_channel_t channel, ledc_timer_t timer) : PIN(pin), CHANNEL(channel), TIMER(timer) {
    }

    void init(uint32_t frequency,
              uint32_t dutyCycle = 0,
              ledc_timer_bit_t dutyResolution = LEDC_TIMER_10_BIT,
              ledc_mode_t speedMode = LEDC_LOW_SPEED_MODE) {
        if (_isInitialized) {
            return;
        }

        _speedMode = speedMode;
        _maxDutyCycle = (1 << dutyResolution) - 1;
        if (dutyCycle > _maxDutyCycle) {
            dutyCycle = _maxDutyCycle;
        }

        ledc_timer_config_t timerConfig = {.speed_mode = _speedMode,
                                           .duty_resolution = dutyResolution,
                                           .timer_num = TIMER,
                                           .freq_hz = frequency,
                                           .clk_cfg = LEDC_AUTO_CLK};

        ledc_timer_config(&timerConfig);

        ledc_channel_config_t channelConfig = {.gpio_num = PIN,
                                               .speed_mode = speedMode,
                                               .channel = CHANNEL,
                                               .timer_sel = TIMER,
                                               .duty = dutyCycle,
                                               .hpoint = 0};

        ledc_channel_config(&channelConfig);

        _isInitialized = true;
    }

    void setDuty(uint32_t dutyCycle) {
        if (!_isInitialized) {
            return;
        }

        ledc_set_duty(_speedMode, CHANNEL, dutyCycle);
        ledc_update_duty(_speedMode, CHANNEL);
    }

    uint32_t getMaxDutyCycle() {
        return _maxDutyCycle;
    }

  private:
    bool _isInitialized = false;

    const int PIN;
    const ledc_channel_t CHANNEL;
    const ledc_timer_t TIMER;

    ledc_mode_t _speedMode{};
    uint32_t _maxDutyCycle{};
};

static PwmController ledPwmController(LED_PWM_PIN, LED_LEDC_CHANNEL, LED_LEDC_TIMER);
static PwmController motorPwmController(MOTOR_PWM_PIN, MOTOR_LEDC_CHANNEL, MOTOR_LEDC_TIMER);

void setup() {
    Serial.begin(115200);

    pinMode(POT_PIN, INPUT);
    analogReadResolution(ANALOG_RESOLUTION);

    ledPwmController.init(LED_LEDC_FREQUENCY);
    motorPwmController.init(MOTOR_LEDC_FREQUENCY);
}

void loop() {
    static SmaFilter potFilter(ADC_SMA_WINDOW);
    static int lastPotValue = -1;

    uint16_t potValue = analogRead(POT_PIN);
    potFilter.addValue(potValue);
    int filteredPotValue = (int)potFilter.getAverage();

    if (abs(filteredPotValue - lastPotValue) > ADC_THRESHOLD) {
        lastPotValue = filteredPotValue;

        int ledDutyCycle = map(filteredPotValue, 0, ANALOG_MAX_VALUE, 0, ledPwmController.getMaxDutyCycle());
        int motorDutyCycle = map(filteredPotValue, 0, ANALOG_MAX_VALUE, 0, motorPwmController.getMaxDutyCycle());
        ledPwmController.setDuty(ledDutyCycle);
        motorPwmController.setDuty(motorDutyCycle);

        Serial.printf("Pot: %4d → Filtered: %4d\n", potValue, filteredPotValue);
        Serial.printf("Led Speed: %3d%% →  Led Duty: %4d\n",
                      (ledDutyCycle * 100) / ledPwmController.getMaxDutyCycle(),
                      ledDutyCycle);
        Serial.printf("Motor Speed: %3d%% →  Motor Duty: %4d\n",
                      (motorDutyCycle * 100) / motorPwmController.getMaxDutyCycle(),
                      motorDutyCycle);
    }

    delay(100);
}
