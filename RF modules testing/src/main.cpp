#include <Arduino.h>

void setup() {
  // Initialize the LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Turn the LED on
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); // Wait for 1 second

  // Turn the LED off
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000); // Wait for 1 second
}
