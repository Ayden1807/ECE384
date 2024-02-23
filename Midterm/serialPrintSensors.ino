void serialPrintSensors() {
  int temperature = 0;
  int humidity = 0;
  int chk = dht.readTemperatureHumidity(temperature, humidity);

  if (chk == 0) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.write(176); // degree symbol
    Serial.println("C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  } else {
    Serial.print("DHT11 Read Error: ");
    Serial.println(chk);
  }
}