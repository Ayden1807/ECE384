void factoryReset(const char* path) {
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

  Serial.println("Factory reset completed. ESP32 restarted in Access Point mode.");
}



