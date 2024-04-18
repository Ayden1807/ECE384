void loadCredentials() {
  File wifiFile = SPIFFS.open(filename, "r");
  if (!wifiFile) {
    Serial.println("Failed to open file for reading");
    return;
  }

  #ifdef DEBUG
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
    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi network. Restarting in Access");
    }
}
