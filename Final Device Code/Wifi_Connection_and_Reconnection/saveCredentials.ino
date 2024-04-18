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
  #ifdef DEBUG
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

