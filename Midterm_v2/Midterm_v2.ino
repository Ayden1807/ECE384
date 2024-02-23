// #define DEBUG_MODE

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT11.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define encoderCLK 18
#define encoderDATA 19
#define encoderSW 13
#define BUTTON_PIN 13
#define DHTPIN  17
#define LIGHT 14

// Core Functionality
bool displayActive = false;
volatile bool buttonPressed = false;

DHT11 dht11(DHTPIN);
Adafruit_BMP280 bmp;

enum Mode {
  ON_REQUEST,
  DEBUG_MODE   // Remove the comma after DEBUG_MODE
};

Mode currentMode = ON_REQUEST;

// LCD
LiquidCrystal_PCF8574 lcd(0x27);

void setup() {
  // Initialize sensors and LCD
  bmp.begin(BMP280_ADDRESS_ALT,  BMP280_CHIPID);
  Serial.begin(9600);
  // Wait for Serial to be available on Leonardo
  while (!Serial);
  delay (100);

  Wire.begin();
  Wire.beginTransmission(0x27);
  int error = Wire.endTransmission();

  Serial.print("Error: ");
  Serial.println(error == 0 ? "LCD found." : "LCD not found.");

  // Initialize pins
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDATA, INPUT);
  pinMode(encoderSW, INPUT);      //INPUTPULLUP
  attachInterrupt(digitalPinToInterrupt(encoderSW), isr, FALLING);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setBacklight(255);

  // OPTIONAL
  Serial.print("Initial Mode: ");
  switch (currentMode) {
    case ON_REQUEST:
      Serial.println("On Request Mode");
      break;
    case DEBUG_MODE:
      Serial.println("Debug Mode");
      break;
    // Add more cases if needed for additional modes
  }
}

void loop() {
  modeSelect();

  // Core Functionality (On Request Mode)
  if (currentMode == ON_REQUEST && buttonPressed) {
    readSensorsAndDisplay();
    buttonPressed = false;
  }

  // Debug Mode
  #ifdef DEBUG_MODE
  if (currentMode == DEBUG_MODE) {
    printDebugData();
  }
  #endif
}

void modeSelect() {
  int encoderPos = 0;
  int aState;
  int aLastState;
  int counter = 0;

  aState = digitalRead(encoderCLK);

  if (aState != aLastState) {
    if (digitalRead(encoderDATA) != aState) {
      encoderPos++;
      counter++;
      if(encoderPos > 1){
        encoderPos = 1;
      }
    } else {
      encoderPos--;
      counter--;
      if (encoderPos < 0) {
        encoderPos = 0;  // Wrap around to the last mode
      }
    }

    Serial.print("Encoder Position: ");
    Serial.println(encoderPos);

    Serial.print("Counter: ");
    Serial.println(counter);

    aLastState = aState;
  }

  if (digitalRead(encoderSW) == LOW) {
    // Button is pressed, switch mode
    currentMode = static_cast<Mode>(encoderPos);
    encoderPos = 0;
    Serial.println("Button pressed!");

    // Optional: Print the current mode to Serial Monitor
    Serial.print("Current Mode: ");
    switch (currentMode) {
      case ON_REQUEST:
        Serial.println("On Request Mode");
        break;
      case DEBUG_MODE:
        Serial.println("Debug Mode");
        break;
      // Add more cases if needed for additional modes
    }
  }
}

void isr() {
  buttonPressed = true;
}

void readSensorsAndDisplay() {
  // Read sensor data and display on LCD
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();
  float pressure = bmp.readPressure() / 100.0;  // Convert Pa to hPa
  int lightIntensity = analogRead(LIGHT);

  // Display sensor data on LCD
  lcd.setBacklight(255);
  lcd.clear();
  lcd.print(temperature);
  lcd.write(223);
  lcd.print("C ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print(pressure);
  lcd.print("hPa ");
  lcd.print(lightIntensity);

  // Print sensor data to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C\t");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa\t");

  Serial.print("Light: ");
  Serial.println(lightIntensity);

  delay(5000);
  lcd.setBacklight(0);
}

#ifdef DEBUG_MODE
void printDebugData() {
  // Print sensor data to the Serial Monitor
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();
  float pressure = bmp.readPressure() / 100.0;  // Convert Pa to hPa
  int lightIntensity = analogRead(LIGHT);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C\t");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa\t");

  Serial.print("Light: ");
  Serial.println(lightIntensity);
  delay(1000); // Print every 1 second
}
#endif
