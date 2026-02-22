#include <Arduino.h>

#define LDR_PIN 4

const float U_REF = 3.329;

const uint8_t ADC_RESOLUTION = 12;
const float ADC_MAX = 4095.0;
// const uint8_t ADC_RESOLUTION = 10;
// const float ADC_MAX = 1023.0;

const adc_attenuation_t ATTENUATION = ADC_ATTENDB_MAX;
// const adc_attenuation_t ATTENUATION = ADC_6db;

void setup()
{
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);
    analogReadResolution(ADC_RESOLUTION);
    analogSetPinAttenuation(LDR_PIN, ATTENUATION);
}

void loop()
{
    uint16_t rawValue = analogRead(LDR_PIN);
    float voltageCalculated = (rawValue / ADC_MAX) * U_REF;
    uint32_t voltageMillivolts = analogReadMilliVolts(LDR_PIN);
    float voltageRead = voltageMillivolts / 1000.0;
    float errorPercent = 0.0;
    if (voltageRead > 0)
    {
        errorPercent = abs(voltageCalculated - voltageRead) / voltageRead * 100.0;
    }

    Serial.printf(
        "| %u | %.3f | %.3f | %.2f%% |\n",
        rawValue,
        voltageCalculated,
        voltageRead,
        errorPercent);

    delay(100);
}
