void factoryReset() {
  // Clear SPIFFS
  SPIFFS.format();

  // Delete WiFi credentials file
  if (SPIFFS.exists(filename)) {
    SPIFFS.remove(filename);
    Serial.println("WiFi credentials file deleted");
  }

  // Restart ESP32 in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("Factory reset completed. ESP32 restarted in Access Point mode.");
}
