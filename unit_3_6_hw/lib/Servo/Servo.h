#ifndef SERVO_H
#define SERVO_H

#include <memory>

#include "PwmController.h"
#include "esp_err.h"

class Servo {
  public:
    static const uint16_t FREQUENCY_HZ = 50;

    Servo(std::unique_ptr<PwmController> pwm);

    esp_err_t setAngle(uint32_t degrees);
    esp_err_t open();
    esp_err_t close();

  private:
    std::unique_ptr<PwmController> _pwm;

    static constexpr uint32_t MIN_PULSE_US = 500;
    static constexpr uint32_t MAX_PULSE_US = 2400;
    static constexpr uint32_t MIN_ANGLE = 0;
    static constexpr uint32_t MAX_ANGLE = 180;

    uint32_t angleToDuty(uint32_t degrees) const;
};

#endif
