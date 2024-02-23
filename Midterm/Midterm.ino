#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT11.h>

#define encoderCLK 18
#define encoderDATA 19
#define encoderSW 13
#define DHTPIN 17
#define BUTTON_PIN 13

// interrupt
volatile int desiredMode = -1;

// modeSelect
int encoderPos = 0;
int counter = 0;
int aState;
int aLastState;  

// BUTTON???
int buttonState = 0;
int lastButtonState = 0;

// from the old code
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

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);

  aLastState = digitalRead(encoderCLK);
}

void modeSelect();
void REGULAR_MODE();

void loop() {
    modeSelect();

    // Switch modes based on encoder position
    switch (encoderPos) {
        case 0:
            currentMode = REGULAR;
            break;
        case 1:
            currentMode = DEBUG;
            break;
        case 2:
            currentMode = CONSTANT_DISPLAY;
            break;
        case 3:
            currentMode = POWER_SAVE;
            break;
        case 4:
            currentMode = ON_REQUEST_SAVE_DATA;
            break;
        case 5:
            currentMode = HIBERNATE_STANDBY;
            break;
        // Add more cases as needed

        // Wrap-around for continuous mode switching
        default:
            encoderPos = 0;
            break;
    }

    // Perform actions based on the current mode
    switch (currentMode) {
        case REGULAR:
            lcd.clear();
            lcd.print("Regular Mode");
            REGULAR_MODE();
            break;
        case DEBUG:
            lcd.clear();
            lcd.print("Debug Mode");
            // Debug mode actions
            break;
        case CONSTANT_DISPLAY:
            lcd.clear();
            lcd.print("Constant Display Mode");
            // Constant display mode actions
            break;
        case POWER_SAVE:
            lcd.clear();
            lcd.print("Power Save Mode");
            // Power save mode actions
            break;
        case ON_REQUEST_SAVE_DATA:
            lcd.clear();
            lcd.print("On Request Mode");
            // On request save data mode actions
            break;
        case HIBERNATE_STANDBY:
            lcd.clear();
            lcd.print("Hibernate/Standby Mode");
            // Hibernate/standby mode actions
            break;
        // Add more cases as needed
    }

    // Add delay if necessary to avoid rapid menu changes
    delay(100);
}

