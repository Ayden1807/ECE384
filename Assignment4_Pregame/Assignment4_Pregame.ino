#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
 
#define analogPin 15
#define interruptPin 4
#define LED_BUILTIN 13
int adcValue = 0;  /* Variable to store Output of ADC */
 
byte degree_symbol[8] =
          	{
            	0b00111,
            	0b00101,
            	0b00111,
            	0b00000,
            	0b00000,
            	0b00000,
            	0b00000,
            	0b00000
          	};
 
LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_BMP280 bmp;
 
struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};
 
Button button1 = {18, 0, false};
 
void IRAM_ATTR isr() {
  button1.numberKeyPresses += 1;
  button1.pressed = true;
}
 
void setup()
{ 
  int error;
  Serial.begin(9600);
  Serial.println("LCD...");
 
  // wait on Serial to be available on Leonardo
  while (!Serial);
 
  Serial.println("Dose: check for LCD");
 
  // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);
  if (error == 0) {
	Serial.println(": LCD found.");
	lcd.begin(16, 2); // initialize the lcd
  } else {
	Serial.println(": LCD not found.");
  } // if
  lcd.setBacklight(255);
  lcd.clear();
  lcd.home();
  lcd.createChar(0, degree_symbol);
  unsigned status;
  status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  // status = bmp.begin();
  if (!status) {
	lcd.print(" BMP280 Sensor  ");
	lcd.setCursor(0,1);
	lcd.print(" not found ! ! !");
	while (1) delay(10);
  }
 
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, 	/* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
 
  lcd.clear();
  lcd.print("PRESS BUTTON!");
  adcAttachPin(analogPin);
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(button1.PIN, isr, FALLING);
}
 
void loop()
{
  if (button1.pressed) {
  	Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
  	button1.pressed = false;
  	blink();
  }
}
void blink() {
  adcValue = analogRead(analogPin); /* Read the Analog Input value */
  lcd.clear();
  lcd.home();
  lcd.setCursor(0,0);
  lcd.print("Pressure=");
  lcd.setCursor(9,0);
  lcd.print(bmp.readPressure());
  lcd.setCursor(13,0);
  lcd.print("hPa");
 
  lcd.setCursor(0,1);
  lcd.print("ADC=");
  lcd.setCursor(4,1);
  lcd.print(adcValue);
  lcd.setCursor(8,1);
  lcd.print("T=");
 
  lcd.setCursor(10,1);
  lcd.print(bmp.readTemperature());
  lcd.setCursor(14,1);
  lcd.print((char)223);
  lcd.setCursor(15,1);
  lcd.print("C");
  delay(1000);
}

