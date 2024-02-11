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

// Create an instance of the DHT11 class.
// - For Arduino: Connect the sensor to Digital I/O Pin 2.
// - For ESP32: Connect the sensor to pin GPIO2 or P2. CANT BOOT WHILE PLUGGED INTO 2
// USE PIN 17 INSTEAD
DHT11 dht11(17);

void setup() {
    Serial.begin(9600);
    
    // DHT-11 SETUP
    // Uncomment the line below to set a custom delay between sensor readings (in milliseconds).
    dht11.setDelay(1000); // Set this to the desired delay. Default is 500ms.

    // BMP280 SETUP
    while ( !Serial ) delay(100);   // wait for native usb
    Serial.println(F("BMP280 test"));
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
    int temperature = 0;
    int humidity = 0;

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("DHT-11\t");
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
        // Serial.print("\tHumidity: ");
        // Serial.print(humidity);
        // Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }

    Serial.print("BMP280\t");
    Serial.print(F("Temperature: "));
    Serial.print(bmp.readTemperature());
    Serial.println("°C");

    // Serial.print(F("Pressure = "));
    // Serial.print(bmp.readPressure());
    // Serial.println(" Pa");

    // Serial.print(F("Approx altitude = "));
    // Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    // Serial.println(" m");

    Serial.println();
    // delay(1000);
}