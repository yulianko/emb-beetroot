#include "Servo.h"

Servo::Servo(std::unique_ptr<PwmController> pwm) : _pwm(std::move(pwm)) {
}

esp_err_t Servo::setAngle(uint32_t degrees) {
    if (degrees > MAX_ANGLE) {
        degrees = MAX_ANGLE;
    }

    return _pwm->setDuty(angleToDuty(degrees));
}

esp_err_t Servo::open() {
    return setAngle(MAX_ANGLE);
}

esp_err_t Servo::close() {
    return setAngle(MIN_ANGLE);
}

uint32_t Servo::angleToDuty(uint32_t degrees) const {
    uint32_t pulseUs = MIN_PULSE_US + (degrees * (MAX_PULSE_US - MIN_PULSE_US)) / MAX_ANGLE;
    uint32_t periodUs = 1'000'000 / _pwm->getFrequency();

    return (pulseUs * _pwm->getMaxDutyCycle()) / periodUs;
}
