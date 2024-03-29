#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_PCF8574.h>

const char *ssid = "ESP32-Access-Point";
const char *password = "123456789";

WebServer server(80);

// Define LCD pins
#define LCD_ADDRESS 0x27
#define LCD_SDA 21
#define LCD_SCL 22
#define LCD_ROWS 16
#define LCD_COLS 2

LiquidCrystal_PCF8574 lcd(LCD_ADDRESS);

void handleRoot() {
  server.send(200, "text/html", "<form action='/submit' method='post'><input type='text' name='ssid'><input type='text' name='password'><input type='submit' value='Submit'></form>");
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

  // Print the new IP address to the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  // Start ESP32 in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
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

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  // Code for other functionalities, if any
}
