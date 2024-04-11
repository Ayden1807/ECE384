#include <WiFi.h> //Wifi library fro Arduino MKR1000 and WiFi shield


// const char* ssid = "Aydens iphone";      // your network SSID (name)
// const char* password = "ayden1807";   // your network password
const char* ssid = "ncsu";      // your network SSID (name)

//int status = WL_IDLE_STATUS; //status of wifi

//WiFiServer server(80); //declare server object and spedify port, 80 is port used for internet

void setup(){
Serial.begin(115200);
delay(1000);

WiFi.mode(WIFI_STA); // Optional
// WiFi.begin(ssid, password);
WiFi.begin(ssid);
Serial.println("\nConnecting");

while(WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  delay(100);
}

Serial.println("\nConnected to the WiFi network");
Serial.print("Local ESP32 IP: ");
Serial.println(WiFi.localIP());

Serial.println("ESP32 MAC:");
Serial.println(WiFi.macAddress());

}
void loop(){}
