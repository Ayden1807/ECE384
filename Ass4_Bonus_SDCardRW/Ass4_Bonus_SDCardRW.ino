#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SDA 21  // SDA pin for BMP280
#define BMP_SCL 22  // SCL pin for BMP280
#define ADC_PIN 4  // ADC pin

Adafruit_BMP280 bmp;

int dataWrites = 0;
const int maxDataWrites = 60;  // Set the maximum number of data writes
bool terminateLoop = false;    // Flag to indicate when to exit the loop

void setup() {
  Serial.begin(115200);

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

  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.print("OK");

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Create CSV file and write header
  if (SD.remove("/data.csv")) {
      Serial.println("OK");
    } else {
      Serial.println("Error deleting data.csv");
    }

  File dataFile = SD.open("/data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time,BMP Temperature (C),BMP Pressure (Pa),ADC Value");
    dataFile.close();
    Serial.println("Header written to file");
  } else {
    Serial.println("Error creating data.csv");
    
  }
}


void loop() {
  // Check the termination flag and exit the loop if necessary
  if (terminateLoop) {
    readAndPrintFile();  // Call the function to read and print the file
    while (1) delay(10);  // Infinite loop to stop further execution
  }

  // Get sensor readings
  float bmpTemperature = bmp.readTemperature();
  float bmpPressure = bmp.readPressure();
  int adcValue = analogRead(ADC_PIN);

  // Get current time
  unsigned long currentTime = millis();

  // Open the file in append mode and write sensor values
  File dataFile = SD.open("/data.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.print(currentTime);
    dataFile.print(",");
    dataFile.print(bmpTemperature);
    dataFile.print(",");
    dataFile.print(bmpPressure);
    dataFile.print(",");
    dataFile.println(adcValue);
    dataFile.close();
    Serial.println("Data written to file");

    dataWrites++;  // Increment the data writes counter

    if (dataWrites >= maxDataWrites) {
      Serial.println("Reached maximum data writes. Setting terminateLoop flag.");
      terminateLoop = true;  // Set the flag to terminate the loop
    }
  } else {
    Serial.println("Error opening data.csv");
  }

  delay(1000); // Adjust the delay according to your sampling rate
}

void readAndPrintFile() {
  // Open the file in read mode and print its content to Serial Monitor
  File dataFile = SD.open("/data.csv", FILE_READ);
  if (dataFile) {
    Serial.println("Reading and printing file contents:");
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.println("Error opening data.csv for reading");
  }
}