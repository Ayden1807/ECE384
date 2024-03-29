#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_PCF8574.h>
#include <DHT11.h>  // Include the DHT library
#include <SPIFFS.h>

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

WebServer server(80);

// Define LCD pins
#define LCD_ADDRESS 0x27
#define LCD_SDA 21
#define LCD_SCL 22
#define LCD_ROWS 16
#define LCD_COLS 2
#define RST 0
#define DHT_PIN 16

LiquidCrystal_PCF8574 lcd(LCD_ADDRESS);
DHT11 dht(DHT_PIN);

const char *filename = "/wifi.txt";

void handleRoot() {
  server.send(200, "text/html", "<form action='/submit' method='post'><input type='text' name='ssid' placeholder='Enter SSID'><br><input type='text' name='password' placeholder='Enter Password'><br><input type='submit' value='Submit'></form>");
}

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
  lcd.begin(LCD_COLS, LCD_ROWS);
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

void loop() {
  
  server.handleClient();

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

  // Debugging: Read and print saved credentials
  Serial.println("Debugging: Reading saved credentials from SPIFFS:");
  File debugFile = SPIFFS.open(filename, "r");
  if (debugFile) {
    Serial.println("File content:");
    while (debugFile.available()) {
      Serial.println(debugFile.readStringUntil('\n'));
    }
    debugFile.close();
  } else {
    Serial.println("Failed to open file for debugging");
  }
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
  while (WiFi.status() != WL_CONNECTED && attempt < 10) { // Try for a maximum of 10 attempts
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
