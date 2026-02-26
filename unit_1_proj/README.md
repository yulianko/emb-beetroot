# Міні-проєкт: підключення сенсора і актуатора, відладчик

## Device Structure

### Mode Controller

|Component | Pin | Connection |
|-|-|-|
| Button Module with pull-down | GPIO 4 | VCC (3.3V) -> Signal ->  GND  |
| Built-in LED  | LED_BUILTIN | |

Перемикання режиму роботи пристрою відбувається циклічно при кожному натисканні кнопки. Внутріній світлодіод - індикатор обраного режиму.

- FORCE_OFF - світлодіод вимкнено
- FORCE_ON - світлодіод світиться
- AUTO - світлодіод блимає

### Sensor

Аналоговий сенсор для визначення рівня освітленості підключений як дільник напруги. Використовує гістерезис для запобігання мерехтінню.

| Component | Pin | Connection | Notes |
|-|-|-|-|
| LDR | GPIO 6 | 3.3V -> **LDR** -> Pin -> Resistor -> GND | High-side Voltage Divider |
| Resistor 10 kΩ | - | 3.3V -> LDR -> Pin -> **Resistor** -> GND | High-side Voltage Divider |

Перемикання актуатора за допомогою сенсора включається в режимі AUTO.

### Actuator

#### Led Actuator

Жовтий зовнішній світлодіод - актуатор.

| Component | Pin | Connection | Notes |
|-|-|-|-|
| Yellow LED | GPIO 16 | Pin -> Resistor -> **Anode** -> **Cathode** -> GND | Signal Output |
| Resistor 360 Ω | GPIO 16 | Pin -> **Resistor** -> Anode -> Cathode -> GND | Current limiting |

Стан актуатора при різних режимах роботи пристрою:

- FORCE_OFF - світлодіод вимкнено
- FORCE_ON - світлодіод світиться
- AUTO - світлодіод міняє стан від спрацьовування сенсору
