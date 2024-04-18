#define DEBUG

#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_PCF8574.h>
#include <SPIFFS.h>

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";
const char *filename = "/wifi.txt";

WebServer server(80);

// Define LCD pins
#define LCD_ADDRESS 0x27
#define LCD_SDA 21
#define LCD_SCL 22
#define LCD_ROWS 16
#define LCD_COLS 2
#define RST 0

LiquidCrystal_PCF8574 lcd(LCD_ADDRESS);



void handleRoot() {
  server.send(200, "text/html", "<form action='/submit' method='post'><input type='text' name='ssid' placeholder='Enter SSID'><br><input type='text' name='password' placeholder='Enter Password'><br><input type='submit' value='Submit'></form>");
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
  delay(5000);
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

  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // Check if WiFi credentials file exists
  // factoryReset(filename);
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

void loop() {
  
  server.handleClient();

  if(digitalRead(RST) == LOW){
    Serial.println("RST Button Pressed");
    factoryReset(filename);
    delay(1000);
  }
  // Code for other functionalities, if any
}

void printIPAddress() {
  lcd.clear(); // Clear LCD before printing
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
}
