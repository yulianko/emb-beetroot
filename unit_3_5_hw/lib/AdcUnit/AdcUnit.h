#ifndef ADC_UNIT_H
#define ADC_UNIT_H

#include "esp_adc/adc_oneshot.h"

class AdcUnit {
  public:
    AdcUnit(adc_unit_t unit_id);

    esp_err_t init();

    esp_err_t getHandle(adc_oneshot_unit_handle_t* handle) const;

    ~AdcUnit();

  private:
    adc_oneshot_unit_handle_t _handle = nullptr;
    const adc_unit_t UNIT_ID;

    bool _isInitialized = false;
};

#endif
