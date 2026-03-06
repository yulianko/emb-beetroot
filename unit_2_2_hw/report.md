
# Завдання 1

## Device Monitor
Button Pressed! Counter: 1

Relay ON

Input Signal: HIGH at 5 ms since last button press

Input Signal: HIGH at 5 ms since last button press

Input Signal: HIGH at 5 ms since last button press

Input Signal: LOW at 15 ms since last button press

Button Pressed! Counter: 2

Relay OFF

Avg wait time for input signal: 15 ms

Input Signal: HIGH at 4 ms since last button press

Input Signal: HIGH at 4 ms since last button press

Button Pressed! Counter: 3

Relay ON

Avg wait time for input signal: 9 ms

Input Signal: HIGH at 4 ms since last button press

Input Signal: HIGH at 4 ms since last button press

Input Signal: LOW at 14 ms since last button press

Button Pressed! Counter: 4

Relay OFF

Avg wait time for input signal: 11 ms

Input Signal: HIGH at 4 ms since last button press

Button Pressed! Counter: 5

Relay ON

Avg wait time for input signal: 9 ms

Input Signal: HIGH at 4 ms since last button press

Input Signal: HIGH at 4 ms since last button press

Input Signal: LOW at 15 ms since last button press

Button Pressed! Counter: 6

Relay OFF

Avg wait time for input signal: 10 ms

Input Signal: HIGH at 5 ms since last button press

Button Pressed! Counter: 7

Relay ON

Avg wait time for input signal: 9 ms

Input Signal: HIGH at 4 ms since last button press

Input Signal: HIGH at 4 ms since last button press

Input Signal: LOW at 14 ms since last button press

Button Pressed! Counter: 8

Relay OFF

Avg wait time for input signal: 10 ms

Input Signal: HIGH at 5 ms since last button press

Input Signal: HIGH at 5 ms since last button press

Button Pressed! Counter: 9

Relay ON

Avg wait time for input signal: 9 ms

Input Signal: HIGH at 4 ms since last button press

Input Signal: HIGH at 4 ms since last button press

Input Signal: LOW at 14 ms since last button press

Button Pressed! Counter: 10

Relay OFF

Avg wait time for input signal: 10 ms

Input Signal: HIGH at 5 ms since last button press

Input Signal: HIGH at 5 ms since last button press

## Висновок

Середній час спрацювання реле становить ~10 мс.

Контакт реле має виражений брязкіт - кожне спрацювання дає 2-4 зайвих імпульси
протягом перших 5 мс. Це типова поведінка механічного реле і потребує програмного
або апаратного усунення брязкоту якщо стан контакту використовується як вхідний сигнал.
