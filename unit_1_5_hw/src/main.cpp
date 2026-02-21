
#include <Arduino.h>

// This button has build in pull-down resistor
#define BUTTON 15
#define DEBOUNCE_DELAY 5

int16_t counter = 0;
unsigned long lastInterruptTime = 0;

void IRAM_ATTR reaction() {
  unsigned long interruptTime = millis();

  // Check if enough time has passed since last interrupt (debounce)
  if (interruptTime - lastInterruptTime > DEBOUNCE_DELAY) {
    counter++;
    Serial.println("\nButton Pressed! Count: " + String(counter));
    lastInterruptTime = interruptTime;
  }

  // counter++;
  // Serial.println("\nButton Pressed! Count: " + String(counter)); 
}

void setup() {
  pinMode(BUTTON, INPUT);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(BUTTON), reaction, FALLING);
}

void loop() {
  Serial.print(".");
  delay(250);
}
