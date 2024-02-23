void turnOff5Sec() {
  if (millis() - displayStartTime > 5000) {
    // Turn off the display and backlight after 5 seconds
    lcd.clear();
    lcd.noDisplay();
    lcd.setBacklight(0);
  }
}
