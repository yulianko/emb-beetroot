#include <Arduino.h>

const int redPin = 18;
const int bluePin = 17;

const int interval = 300;

void setup() {
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(bluePin, LOW);

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  digitalWrite(redPin, HIGH);
  digitalWrite(bluePin, LOW);
  delay(interval);

  digitalWrite(redPin, LOW);
  digitalWrite(bluePin, HIGH);
  delay(interval);
}