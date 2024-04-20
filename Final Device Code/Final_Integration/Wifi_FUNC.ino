void printIPAddress();

void handleRoot() {
  server.send(200, "text/html", "<form action='/submit' method='post'><input type='text' name='ssid' placeholder='Enter SSID'><br><input type='text' name='password' placeholder='Enter Password'><br><input type='submit' value='Submit'></form>");
}

// ***********************************************************************************************************

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

// ***********************************************************************************************************

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

// ***********************************************************************************************************

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
  #ifdef DEBUG_WIFI
    Serial.println("Debugging: Actual saved SPIFFS files:");
    File debugFile = SPIFFS.open(filename, "r");
    if (debugFile) {
      Serial.println("File content:");
      while (debugFile.available()) {
        Serial.println(debugFile.readStringUntil('\n'));
      }
      debugFile.close();
    } else {
      Serial.println("Failed to open file for debugging");
      debugFile.close();
    }
  #endif

}

// ***********************************************************************************************************

void loadCredentials() {
  File wifiFile = SPIFFS.open(filename, "r");
  if (!wifiFile) {
    Serial.println("Failed to open file for reading");
    return;
  }

  #ifdef DEBUG_WIFI
    Serial.println("Debugging: Loading SPIFFS files for reconnection:");
    File debugFile = SPIFFS.open(filename, "r");
    if (debugFile) {
      Serial.println("File content:");
      while (debugFile.available()) {
        Serial.println(debugFile.readStringUntil('\n'));
      }
      debugFile.close();
    } else {
      Serial.println("Failed to open file for debugging");
      debugFile.close();
    }
  #endif

  Serial.println("Credentials Saved:");
  String ssidMemory = wifiFile.readStringUntil('\n');
  ssidMemory.trim();
  Serial.print("ssid: ");
  Serial.println(ssidMemory);
  String passwordMemory = wifiFile.readStringUntil('\n');
  passwordMemory.trim();
  Serial.print("password: ");
  Serial.println(passwordMemory);
  // Close the file before attempting to connect to WiFi
  wifiFile.close();

  Serial.print("Connecting to stored WiFi network: ");
  Serial.println(ssidMemory);

  // Connect to the stored WiFi network
  const char *char_ssidMemory = ssidMemory.c_str();
  const char *char_passwordMemory = passwordMemory.c_str();
  
  // wifiConnect(ssidMemory, passwordMemory);

  WiFi.disconnect();
  delay(1000);
  int attempt = 0;
  WiFi.begin(char_ssidMemory, char_passwordMemory);
  while (WiFi.status() != WL_CONNECTED && attempt < 30) { // Try for a maximum of 10 attempts
    delay(500);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setBacklight(255);
    lcd.setCursor(0,0);
    lcd.print("Wifi Connected");
    lcd.setCursor(0,1);
    lcd.print("IP: ");
    lcd.print(WiFi.localIP());
    delay(5000);
    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi network. Restarting in Access");
    }
}

// ***********************************************************************************************************

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

// ***********************************************************************************************************

void wifiFactoryReset(const char* path) {
  // Clear the contents of the specified file
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }
    file.close();
    Serial.println("File cleared successfully");
  } else {
    Serial.println("File does not exist");
    return;
  }

  // Restart ESP32 in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("Wifi factory reset completed. ESP32 restarted in Access Point mode.");
}

// ***********************************************************************************************************

void clearWifiMemory(const char* path) {
  // Clear the contents of the specified file
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      lcd.clear();
      lcd.setBacklight(255);
      lcd.setCursor(0,0);
      lcd.print("Failed to");
      lcd.setCursor(0,1);
      lcd.print("open file");
      delay(5000);
      return;
    }
    file.close();
    Serial.println("File cleared successfully");
    lcd.clear();
    lcd.setBacklight(255);
    lcd.setCursor(0,0);
    lcd.print("Wifi memory");
    lcd.setCursor(0,1);
    lcd.print("cleared");
    delay(5000);
  } else {
    Serial.println("File does not exist");
    lcd.clear();
    lcd.setBacklight(255);
    lcd.setCursor(0,0);
    lcd.print("File doesn't");
    lcd.setCursor(0,1);
    lcd.print("exist");
    delay(5000);
    return;
  }

  Serial.println("Wifi memory cleared");
}