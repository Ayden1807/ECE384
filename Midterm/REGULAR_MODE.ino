void printSensorsToLCD();
void turnOff5Sec();

void REGULAR_MODE() {
  if (button1.pressed) {
    button1.pressed = false;
    lcd.setBacklight(225);
    printSensorsToLCD();
  }

  delay(5000);
  turnOff5Sec();
}
