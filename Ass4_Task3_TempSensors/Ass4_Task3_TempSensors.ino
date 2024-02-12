// Include the DHT11 library for interfacing with the sensor.
#include <DHT11.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp; // I2C
DHT11 dht11(17);

unsigned long previousMillis = 0;
unsigned long interval = 1000; // 1 second interval for reading sensors
unsigned long errorCalculationInterval = 60000; // 1 minute for calculating average error
unsigned long lastErrorCalculationTime = 0;
int cookie = 0;

float totalTemperatureError = 0.0;
int errorCount = 0;

void setup() {
    Serial.begin(9600);
    
    // DHT-11 SETUP
    // Uncomment the line below to set a custom delay between sensor readings (in milliseconds).
    dht11.setDelay(1000); // Set this to the desired delay. Default is 500ms.

    // BMP280 SETUP
    while ( !Serial ) delay(100);   // wait for native usb
    Serial.println(F("BMP280 test and DHT11 Sensor Readings"));
    unsigned status;
    status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
    // status = bmp.begin();
    if (!status) {
      Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
      Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
    unsigned long currentMillis = millis();

    // Read sensors every 1 second
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        int temperatureDHT11 = 0;
        int humidity = 0;

        int result = dht11.readTemperatureHumidity(temperatureDHT11, humidity);

        if (result == 0) {
            Serial.print("DHT-11\t");
            Serial.print("Temperature: ");
            Serial.print(temperatureDHT11);
            Serial.println("°C");
        } else {
            Serial.println(DHT11::getErrorString(result));
        }

        Serial.print("BMP280\t");
        Serial.print(F("Temperature: "));
        float temperatureBMP = bmp.readTemperature();
        Serial.print(temperatureBMP);
        Serial.println("°C");

        // Calculate temperature difference
        float temperatureDifference = temperatureDHT11 - temperatureBMP;
        // Serial.print("Temperature Difference: ");
        // Serial.print(temperatureDifference);
        // Serial.println("°C");

        // cookie++;  // Counter to make sure correct number of samples was being averaged
        // Serial.print("Counter: ");
        // Serial.println(cookie);
        
        // Calculate average temperature error every 1 minute
        if (currentMillis - lastErrorCalculationTime >= errorCalculationInterval) {
            lastErrorCalculationTime = currentMillis;

            // Update total temperature error and error count
            totalTemperatureError += abs(temperatureDifference);
            errorCount++;

            // Calculate and print average temperature error
            if (errorCount > 0) {
                float averageError = totalTemperatureError / errorCount;
                Serial.print("Average Temperature Error: ");
                Serial.print(averageError);
                Serial.println("°C");
            }
        }

        Serial.println();
    }
}