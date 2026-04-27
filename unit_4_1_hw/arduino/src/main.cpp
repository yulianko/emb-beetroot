#include <Arduino.h>
#include <HardwareSerial.h>
#include <OneButton.h>

#ifdef BOARD_ESP

static constexpr char NAME[] = "ESP";

static constexpr uint8_t BTN_PIN = 0;

static constexpr uint8_t LED_PIN = 4;

static constexpr uint8_t UART1_TX = 17;
static constexpr uint8_t UART1_RX = 18;

static constexpr char TX_COMMAND[] = "STM_TOGGLE";
static constexpr char RX_COMMAND[] = "ESP_TOGGLE";
static constexpr size_t MAX_COMMAND_SIZE = 16;

#else
#ifdef BOARD_STM

static constexpr char NAME[] = "STM";

static constexpr uint8_t BTN_PIN = PA0;

static constexpr uint8_t LED_PIN = PC13;

static constexpr uint8_t UART1_TX = PA9;
static constexpr uint8_t UART1_RX = PA10;

static constexpr char TX_COMMAND[] = "ESP_TOGGLE";
static constexpr char RX_COMMAND[] = "STM_TOGGLE";
static constexpr size_t MAX_COMMAND_SIZE = 16;

#endif
#endif

// Initialize UART
#ifdef BOARD_ESP
HardwareSerial uart1(1);
#else
#ifdef BOARD_STM
HardwareSerial uart1(UART1_RX, UART1_TX);
#endif
#endif

OneButton button(BTN_PIN, true);  // true = active low (internal pullup)

void handleClick() {
    Serial.printf("[%s] Button Pressed\n", NAME);
    uart1.println(TX_COMMAND);
}

void setup() {
    Serial.begin(115200);

    // UART begin
#ifdef BOARD_ESP
    uart1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
#else
#ifdef BOARD_STM
    uart1.begin(9600, SERIAL_8N1);
#endif
#endif

    button.attachClick(handleClick);

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    button.tick();

    if (uart1.available()) {
        static char buffer[MAX_COMMAND_SIZE];
        static size_t position = 0;
        char c = uart1.read();
        if (c == '\r' || c == '\n' || position >= MAX_COMMAND_SIZE - 1) {
            buffer[position] = '\0';
            Serial.printf("[%s] DATA: '%s'\n", NAME, buffer);

            if (strcmp(buffer, RX_COMMAND) == 0) {
                digitalWrite(LED_PIN, !digitalRead(LED_PIN));
                Serial.printf("[%s] Action: Toggle LED\n", NAME);
            }

            position = 0;
        } else {
            buffer[position++] = c;
        }
    }

    delay(10);
}
