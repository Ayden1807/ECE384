// ************************************************************
// FIX THIS!!!!! 34, 35, 36, 39 ARE INPUT ONLY!!!!!
// Status_LED1 --> Blue? = Pin 36
// Status_LED2 --> White? = Pin 39
// Status_LED3 --> Red? = Pin 34
// ************************************************************

// Include the required libraries
#include <Arduino.h>

// Define the onboard LED pin
const int onboardLED = 2; // Assuming onboard LED is connected to pin 2

// Function to blink the onboard LED
void blinkOnboardLED(int blinkDuration, int blinkCount) {
  pinMode(onboardLED, OUTPUT); // Set the LED pin as output

  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(onboardLED, HIGH); // Turn on the LED
    delay(blinkDuration); // Wait for blink duration
    digitalWrite(onboardLED, LOW); // Turn off the LED
    delay(blinkDuration); // Wait for blink duration
  }
}


// Function to turn on the onboard LED continuously
void turnOnOnboardLED() {
  pinMode(onboardLED, OUTPUT); // Set the LED pin as output
  digitalWrite(onboardLED, HIGH); // Turn on the LED
}

// Function to turn off the onboard LED
void turnOffOnboardLED() {
  pinMode(onboardLED, OUTPUT); // Set the LED pin as output
  digitalWrite(onboardLED, LOW); // Turn off the LED
}

// Function to blink the onboard LED with a delay
void blinkOnboardLEDDelay(int blinkDuration) {
  pinMode(onboardLED, OUTPUT); // Set the LED pin as output
  digitalWrite(onboardLED, HIGH); // Turn on the LED
  delay(blinkDuration); // Wait for blink duration
  digitalWrite(onboardLED, LOW); // Turn off the LED
  delay(blinkDuration); // Wait for blink duration
}

// Function to blink the onboard LED with a delay and blink count
void blinkOnboardLEDDelayCount(int blinkDuration, int blinkCount) {
  pinMode(onboardLED, OUTPUT); // Set the LED pin as output

  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(onboardLED, HIGH); // Turn on the LED
    delay(blinkDuration); // Wait for blink duration
    digitalWrite(onboardLED, LOW); // Turn off the LED
    delay(blinkDuration); // Wait for blink duration
  }
}
