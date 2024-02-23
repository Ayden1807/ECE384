#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT11.h>

#define DHTPIN 17
#define BUTTON_PIN 5      // Change to pin 5 for the button
#define LED_BUILTIN 13

DHT11 dht(DHTPIN);
LiquidCrystal_PCF8574 lcd(0x27);

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = {BUTTON_PIN, 0, false};  // Change the PIN to BUTTON_PIN
unsigned long displayStartTime = 0;

void IRAM_ATTR isr() {
  button1.numberKeyPresses += 1;
  button1.pressed = true;
}

void setup() {
  int error;
  float temperature = 0;
  float humidity = 0;

  Serial.begin(9600);

  // Wait for Serial to be available on Leonardo
  while (!Serial);

  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();

  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
    lcd.begin(16, 2);
  } else {
    Serial.println(": LCD not found.");
  }

  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.print("PRESS BUTTON!");

  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(button1.PIN, isr, FALLING);
}

void loop() {
  if (button1.pressed) {
    button1.pressed = false;
    lcd.setBacklight(225);
    displayTemperatureAndHumidity();
  }

  if (millis() - displayStartTime > 5000) {
    // Turn off the display and backlight after 5 seconds
    lcd.clear();
    lcd.noDisplay();
    lcd.setBacklight(0);
  }
}

void displayTemperatureAndHumidity() {
  lcd.clear();
  lcd.display();

  int temperature = 0;
  int humidity = 0;
  int chk = dht.readTemperatureHumidity(temperature, humidity);

  if (chk == 0) {
    lcd.home();
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.write(223);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
  } else {
    Serial.print("DHT11 Read Error: ");
    Serial.println(chk);
  }

  // Save the current time for display timeout
  displayStartTime = millis();
}
