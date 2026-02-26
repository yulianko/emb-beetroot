# Міні-проєкт: підключення сенсора і актуатора, відладчик

## Device Structure

### Mode Controller

|Component | Pin | Connection |
|-|-|-|
| Button Module with pull-down | GPIO 4 | VCC (3.3V), GND, Signal |
| Built-in LED  | LED_BUILTIN | |

Перемикання режиму відбувається циклічно при кожному натисканні кнопки.

- FORCE_OFF (світлодіод вимкнено)
- FORCE_ON (світлодіод: світиться)
- AUTO (світлодіод: блимає)
