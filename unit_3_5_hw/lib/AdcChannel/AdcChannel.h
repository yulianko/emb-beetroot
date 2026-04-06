#ifndef ADC_CHANNEL_H
#define ADC_CHANNEL_H

#include "AdcUnit.h"
class AdcChannel {
  public:
    AdcChannel(AdcUnit& unit, adc_channel_t channel);

    esp_err_t init(adc_atten_t atten = ADC_ATTEN_DB_12, adc_bitwidth_t bitwidth = ADC_BITWIDTH_12);

    esp_err_t readRaw(int* raw);

  private:
    AdcUnit& _unit;
    adc_channel_t _channel;
    bool _isInitialized = false;
};

#endif
