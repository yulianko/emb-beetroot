#include "driver/gpio.h"
#include "esp_timer.h"

class Button {
  public:
    Button(gpio_num_t pin);

    esp_err_t init();
    void attachClick(void (*callback)(void*), void* arg);
    void tick();

  private:
    enum class State {
        IDLE,
        DEBOUNCING,
        PRESSED,
    };

    static constexpr uint32_t DEBOUNCE_US{50 * 1000};

    const gpio_num_t PIN;

    State _state{State::IDLE};
    int64_t _debounceStart{0};
    bool _isInitialized{false};

    void (*_onClickHandler)(void*){nullptr};
    void* _onClickArg{nullptr};
};
