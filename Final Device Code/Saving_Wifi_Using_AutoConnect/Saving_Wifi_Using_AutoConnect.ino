#include <WiFi.h>
#include <LiquidCrystal_PCF8574.h>
#include <SPIFFS.h>
#include <AutoConnect.h>

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

AutoConnect autoConnect;
LiquidCrystal_PCF8574 lcd(0x27);  // Adjust the LCD address if needed
#define RST 0

const char *filename = "/wifi.txt";

void handleRoot() {
  String content = 
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<title>ESP32 WiFi Setup</title>"
  "</head>"
  "<body>"
  "<h1>ESP32 WiFi Setup</h1>"
  "<form action='/submit' method='post'>"
  "<label for='ssid'>SSID:</label>"
  "<input type='text' id='ssid' name='ssid' required><br><br>"
  "<label for='password'>Password:</label>"
  "<input type='password' id='password' name='password' required><br><br>"
  "<input type='submit' value='Submit'>"
  "</form>"
  "</body>"
  "</html>";

  autoConnect.send(content);
}

void handleSubmit() {
  String ssid = autoConnect->readFormArg("ssid");
  String password = autoConnect->readFormArg("password");

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

void setup() {
  Serial.begin(115200);

  pinMode(RST, INPUT_PULLUP);

  // Start ESP32 in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  Serial.println("Access Point Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize LCD
  lcd.begin(16, 2); // Adjust if your LCD has different dimensions
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print("AP IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.softAPIP());

  // Initialize SPIFFS
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

  autoConnect.begin();
  autoConnect.setAutoReconnect(true);
  autoConnect.on("/", [](AutoConnectAux& aux, PageArgument& args) {
    handleRoot();
    return String();
  });
  autoConnect.on("/submit", [](AutoConnectAux& aux, PageArgument& args) {
    handleSubmit();
    return String();
  });


  Serial.println("HTTP server started");
}

void loop() {
  autoConnect.handleClient();

  if(digitalRead(RST) == LOW){
    Serial.println("RST Button Pressed");
    factoryReset();
    delay(1000);
  }
  // Code for other functionalities, if any
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
  Serial.println(ssid);

  // Connect to the stored WiFi network
  WiFi.begin(ssid.c_str(), password.c_str());
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 10) {
    delay(3000);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi network. Restarting in Access Point mode.");
    factoryReset();
  }
}

void printIPAddress() {
  lcd.clear(); // Clear LCD before printing
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
}
