void printIPAddress();
float returnTemp();
float returnPress();
float returnHumid();
String returnLight();

// Moving modes to seperate tabs
void setupRequestMode();
void requestMode();


void handleRoot() {
  server.send(200, "text/html", "<form action='/submit' method='post'><input type='text' name='ssid' placeholder='Enter SSID'><br><input type='text' name='password' placeholder='Enter Password'><br><input type='submit' value='Submit'></form>");
}

// ***********************************************************************************************************

void handleTemp() {
  float temperature = returnTemp();

  if (!isnan(temperature)) {  // Check if the temperature reading is valid
    String response = String(temperature, 2);  // Round temperature to 2 decimal places
    server.send(200, "text/html; charset=utf-8", response);  // Send temperature as response with UTF-8 encoding
  } else {
    server.send(500, "text/plain", "Error: Failed to read temperature data");  // Send an error response
  }
}

// ***********************************************************************************************************

void handlePress() {
  float pressure = returnPress();

  if (!isnan(pressure)) {  // Check if the pressure reading is valid
    String response = String(pressure, 2);  // Round pressure to 2 decimal places
    server.send(200, "text/html; charset=utf-8", response);  // Send pressure as response with UTF-8 encoding
  } else {
    server.send(500, "text/plain", "Error: Failed to read pressure data");  // Send an error response
  }
}

// ***********************************************************************************************************

void handleHumid() {
  float humidity = returnHumid();

  if (!isnan(humidity)) {  // Check if the humidity reading is valid
    String response = String(humidity, 2);  // Round humidity to 2 decimal places
    server.send(200, "text/html; charset=utf-8", response);  // Send humidity as response with UTF-8 encoding
  } else {
    server.send(500, "text/plain", "Error: Failed to read humidity data");  // Send an error response
  }
}

// ***********************************************************************************************************

void handleLight() {
  String light = returnLight();

  if (light == "V.Dark" || light == "Dark" || light == "Bright" || light == "V.Bright") {  // Check if the light reading is valid
    String response = light;
    server.send(200, "text/html; charset=utf-8", response);  // Send light as response with UTF-8 encoding
  } else {
    server.send(500, "text/plain", "Error: Failed to read light data");  // Send an error response
  }
}

// ***********************************************************************************************************
// http://<ESP32_IP_ADDRESS>/setMode?mode=<MODE_NAME>
void handleSetMode() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    
    // Execute code based on the mode
    if (mode == "Request") {
      Serial.println("REQUEST MODE VIA WIFI");
      LCDML_DISP_jumpToFunc(LCDML_FUNC_request_mode);
      //  Code for mode 1

    } else if (mode == "Constant") {
      Serial.println("CONSTANT MODE VIA WIFI");
      LCDML_DISP_jumpToFunc(LCDML_FUNC_constant_mode);
      // Code for mode 2
    } else if (mode == "wifiClear") {
      Serial.println("CLEAR WIFI MEMORY VIA WIFI");
      LCDML_DISP_jumpToFunc(LCDML_FUNC_wifi_clear_memory);
      // Code for mode 3
    } else if (mode == "factoryReset") {
      Serial.println("FACTORY RESET VIA WIFI");
      LCDML_DISP_jumpToFunc(LCDML_FUNC_factory_reset);
      // Code for mode 4
    } else if (mode == "Debug") {
      Serial.println("DEBUG TOGGLED VIA WIFI");
      LCDML_DISP_jumpToFunc(LCDML_FUNC_debug_mode);
      //Code for mode 5 
    // } else if (mode == "Debug") {
    // Serial.println("DEBUG TOGGLED VIA WIFI");
    // LCDML_DISP_jumpToFunc(LCDML_FUNC_debug_mode);
    }

    server.send(200, "text/plain", "Mode set to " + mode);
  } else {
    server.send(400, "text/plain", "Missing mode parameter");
  }
}

// ***********************************************************************************************************
// This function is not currenly used!!!

void handleRMM() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  WiFiClient client = server.client();

  Serial.println("New client connected");

  if (!client.connected()) {
    Serial.println("Client disconnected");
    return;
  }

  // Read the HTTP request
  String httpRequest = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      httpRequest += c;
      if (c == '\n') {
        break;
      }
    }
  }

  Serial.println("Full HTTP request:");
  Serial.println(httpRequest);

  // Read file contents
  File dataFile = SD.open("/constantMemory.csv", FILE_READ);
  
  if (dataFile) {
    String httpResponse = "HTTP/1.1 200 OK\r\n";
    httpResponse += "Content-Type: text/csv\r\n";
    httpResponse += "Connection: close\r\n\r\n";

    // Read file contents and append to HTTP response
    while (dataFile.available()) {
      httpResponse += (char)dataFile.read();
    }

    dataFile.close();

    // Send HTTP response
    client.print(httpResponse);
    Serial.println("HTTP response sent");
  } else {
    String httpResponse = "HTTP/1.1 404 Not Found\r\n";
    httpResponse += "Connection: close\r\n\r\n";
    httpResponse += "File not found";

    // Send HTTP response
    client.print(httpResponse);
    Serial.println("File not found response sent");
  }

  // Close the connection
  client.stop();
  Serial.println("Client disconnected");
}

void handleNotFound() {
  String message = "Not found: " + server.uri();
  Serial.println(message);
  server.send(404, "text/plain", message);
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

// ***********************************************************************************************************

void checkWifi