void printSensorsToLCD() {
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
    lcd.print("DHT11 Read Error: ");
    lcd.println(chk);
  }
}
