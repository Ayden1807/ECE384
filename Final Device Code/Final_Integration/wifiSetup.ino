void handleRoot();
void handleSubmit();
void handleTemp();
void loadCredentials();
void saveCredentials();
void printIPAddress();
void factoryReset(const char* path);
void clearWifiMemory(const char* path);

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
  lcd.setCursor(0,0);
  lcd.print("Access Point");
  lcd.setCursor(0,1);
  lcd.print("Mode");
  delay(5000);
  // blinkOnboardLED(500,10);     //Make this blink until wifi is connected and don't use pin 2 for LED. Pin 2 needs to be connected to the encoder!!
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AP IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.softAPIP());
  delay(10000);

  // Setup server routes
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.on("/temp", handleTemp);  // New route for temperature data

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // factoryReset();       //This is a debug feature

  File file = SPIFFS.open(filename, "r");
  if (file) {
      if (file.available()) {
          Serial.println("Found existing WiFi credentials file with content");
          loadCredentials();
      } else {
          Serial.println("Found existing WiFi credentials file, but it's empty");
      }
      file.close();
  } else {
      Serial.println("No existing WiFi credentials file found");
  }

  server.begin();
  Serial.println("HTTP server started");
}

void apSetup(){
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  Serial.println("Access Point Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.on("/temp", handleTemp);  // New route for temperature data

  server.begin();
  Serial.println("HTTP server started");
}

void wifiConnect(){
  File file = SPIFFS.open(filename, "r");
  if (file) {
      if (file.available()) {
          Serial.println("Found existing WiFi credentials file with content");
          loadCredentials();
      } else {
          Serial.println("Found existing WiFi credentials file, but it's empty");
          wifiSetup();
      }
      file.close();
  } else {
      Serial.println("No existing WiFi credentials file found");
      wifiSetup();
  }
}