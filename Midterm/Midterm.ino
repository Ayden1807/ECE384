#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT11.h>

#define encoderCLK 18
#define encoderDATA 19
#define encoderSW 13
#define DHTPIN 17
#define BUTTON_PIN 13

// modeSelect
int encoderPos = 0;
int aState;
int aLastState;
int counter = 0;
int displayStartTime = 0;

// BUTTON
volatile bool buttonPressed = false;

// from the old code
DHT11 dht(DHTPIN);
LiquidCrystal_PCF8574 lcd(0x27);

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = {BUTTON_PIN, 0};

volatile int desiredMode = -1; // Initialize to an invalid mode

// Define different modes
enum Mode {
  REGULAR,
  DEBUG,
  CONSTANT_DISPLAY,
  POWER_SAVE,
  ON_REQUEST_SAVE_DATA,
  HIBERNATE_STANDBY
};

Mode currentMode = REGULAR;

void IRAM_ATTR isr() {
  button1.numberKeyPresses += 1;
  buttonPressed = true;

  // Set the desired mode based on the encoder position
  switch (encoderPos) {
    case 0:
      desiredMode = REGULAR;
      break;
    case 1:
      desiredMode = DEBUG;
      break;
    case 2:
      desiredMode = CONSTANT_DISPLAY;
      break;
    case 3:
      desiredMode = POWER_SAVE;
      break;
    case 4:
      desiredMode = ON_REQUEST_SAVE_DATA;
      break;
    case 5:
      desiredMode = HIBERNATE_STANDBY;
      break;
    // Add more cases as needed
  }
}

void setup() {
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDATA, INPUT);
  pinMode(encoderSW, INPUT);

  // from the old code
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

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr, FALLING);

  aLastState = digitalRead(encoderCLK);
}



void loop() {
  modeSelect();

  if (desiredMode != -1) {
    // Switch to the desired mode
    currentMode = static_cast<Mode>(desiredMode);
    desiredMode = -1; // Reset the desired mode
    lcd.clear();
  }

  // Switch modes based on encoder position
  switch (currentMode) {
    case REGULAR:
      lcd.println("Regular Mode");
      // Call your regular mode function here
      break;
    case DEBUG:
      lcd.println("Debug Mode");
      // Call your debug mode function here
      break;
    case CONSTANT_DISPLAY:
      lcd.println("Constant Display Mode");
      // Call your constant display mode function here
      break;
    case POWER_SAVE:
      lcd.println("Power Save Mode");
      // Call your power save mode function here
      break;
    case ON_REQUEST_SAVE_DATA:
      lcd.println("On Request Mode");
      // Call your on request save data mode function here
      break;
    case HIBERNATE_STANDBY:
      lcd.println("Hibernate/Standby Mode");
      // Call your hibernate/standby mode function here
      break;
    // Add more cases as needed
  }
  
  // Add delay if necessary to avoid rapid menu changes
delay(1000);
lcd.clear();
}
