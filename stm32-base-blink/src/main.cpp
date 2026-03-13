#include <Arduino.h>

#define LED_PIN PC13
void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  digitalWrite(LED_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  delay(700);
  Serial.println("LED blinked!");
}
