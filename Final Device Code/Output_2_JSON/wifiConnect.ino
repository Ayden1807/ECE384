// void wifiConnect(String ssidMem, String passwordMem){
//   WiFi.begin(ssidMem, passwordMem);
//     while (WiFi.status() != WL_CONNECTED) {
//       delay(500);
//       Serial.print(".");
//     }

//     if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("");
//     Serial.println("WiFi connected");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
//   } else {
//     Serial.println("");
//     Serial.println("Failed to connect to WiFi network. Restarting in Access Point mode.");
//     factoryReset(filename);
//   }
// }