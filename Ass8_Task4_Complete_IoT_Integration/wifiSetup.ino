void wifiSetup() {

  pinMode(RST, INPUT_PULLUP);

  // Start ESP32 in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  Serial.println("Access Point Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Initialize LCD
  lcd.begin(_LCDML_DISP_cols, _LCDML_DISP_rows);
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print("AP IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.softAPIP());

  // Setup server routes
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.on("/temp", handleTemp);  // New route for temperature data

  // Initialize SPIFFS
  // factoryReset();       //This is a debug feature

  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // Check if WiFi credentials file exists
  if (SPIFFS.exists(filename)) {
    Serial.println("Found existing WiFi credentials file");
    loadCredentials();
  } else {
    Serial.println("No existing WiFi credentials file found");
  }

  server.begin();
  Serial.println("HTTP server started");
}