void handleRoot() {
  server.send(200, "text/html", "<form action='/submit' method='post'><input type='text' name='ssid' placeholder='Enter SSID'><br><input type='text' name='password' placeholder='Enter Password'><br><input type='submit' value='Submit'></form>");
}

void handleTemp() {
  float temperature = dht.readTemperature();  // Read temperature from DHT sensor

  if (!isnan(temperature)) {  // Check if the temperature reading is valid
    String response = "<html><head><meta charset='UTF-8'><style>body { font-size: 24px; }</style></head><body>";
    response += "Temperature: " + String(temperature, 2) + "°C";  // Round temperature to 2 decimal places
    response += "</body></html>";
    server.send(200, "text/html; charset=utf-8", response);  // Send temperature as response with UTF-8 encoding
  } else {
    server.send(500, "text/plain", "Error: Failed to read temperature data");  // Send an error response
  }
}

void handleSubmit() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  // Connect to the submitted WiFi network
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Stop Access Point mode
  WiFi.mode(WIFI_STA);
  WiFi.softAPdisconnect(true);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Save the new SSID and password to SPIFFS
  saveCredentials(ssid, password);

  // Print the new IP address to the LCD
  printIPAddress();
}

void saveCredentials(String ssid, String password) {
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Save SSID and password to the file
  file.println(ssid);
  file.println(password);

  // Close the file
  file.close();

  Serial.println("WiFi credentials saved to SPIFFS");

  // Debugging: Read and print saved credentials
  Serial.println("Debugging: Reading saved credentials from SPIFFS:");
  File debugFile = SPIFFS.open(filename, "r");
  if (debugFile) {
    Serial.println("File content:");
    while (debugFile.available()) {
      Serial.println(debugFile.readStringUntil('\n'));
    }
    debugFile.close();
  } else {
    Serial.println("Failed to open file for debugging");
  }
}


void loadCredentials() {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Credentials Saved:");
  String ssid = file.readStringUntil('\n');
  Serial.print("ssid: ");
  Serial.println(ssid);
  String password = file.readStringUntil('\n');
  Serial.print("password: ");
  Serial.println(password);

  // Close the file before attempting to connect to WiFi
  file.close();

  Serial.print("Connecting to stored WiFi network: ");
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0,0);
  lcd.print("Attempting");
  lcd.setCursor(0,1);
  lcd.print("Re-Connect");
  delay(3000);
  Serial.println(ssid);

  // Connect to the stored WiFi network
  WiFi.begin(ssid.c_str(), password.c_str());
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 10) { // Try for a maximum of 10 attempts
    delay(3000);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    turnOnOnboardLED();
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi network. Restarting in Access Point mode."); 
    factoryReset();
    lcd.clear();
    lcd.setBacklight(255);
    lcd.setCursor(0,0);
    lcd.print("WiFi Connection");
    lcd.setCursor(0,1);
    lcd.print("Failed");
    delay(3000);
  }
}

void printIPAddress() {
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  delay(5000);
  lcd.clear(); // Clear LCD before printing
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(10000);
}

void factoryReset() {
  // Clear SPIFFS
  SPIFFS.format();

  // Delete WiFi credentials file
  if (SPIFFS.exists(filename)) {
    SPIFFS.remove(filename);
    Serial.println("WiFi credentials file deleted");
  }

  // Restart ESP32 in Access Point mode
  wifiSetup();

  Serial.println("Factory reset completed. ESP32 restarted in Access Point mode.");
}