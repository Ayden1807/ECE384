void handleSensors() {
  JsonDocument sensorDoc;
  float temperature = dht.readTemperature();  // Read temperature from DHT sensor
  float humidity = dht.readHumidity();

  sensorDoc["DHT_temp"] = temperature;
  sensorDoc["DHT_humidity"] = humidity;

  String jsonString;
  serializeJson(sensorDoc, jsonString);  // Serialize JSON document to a string

  // Send JSON data to a web server (replace with your server URL)
  sendSensorDataToServer(jsonString);

  server.send(200, "application/json", jsonString);
}

void sendSensorDataToServer(String jsonData) {
  // Implement HTTP POST or PUT request to send jsonData to a web server
  // Use WiFiClient and HTTPClient libraries for making HTTP requests
}
