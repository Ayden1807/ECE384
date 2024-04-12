#include <DHT11.h>
#include <Wire.h>
#define DHTPIN 16

DHT11 dht11(DHTPIN);

void setup() {
  Serial.begin(115200);

}

void loop() {
  int temp = 0;
  int humidity = 0;
  dht11.readTemperatureHumidity(temp, humidity);
  Serial.print("DHT11 temp: ");
  Serial.print(temp);
  Serial.print("\tDHT11 humidity: ");
  Serial.println(humidity);
  delay(1000);
}
