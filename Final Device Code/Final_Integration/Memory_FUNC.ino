void setupRequestMemory(){
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
  if (SD.remove("/requestMemory.csv")) {
      Serial.println("OK");
    } else {
      Serial.println("Error deleting requestMemory.csv");
    }

  File dataFile = SD.open("/requestMemory.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time (s),DHT Temperature (C),BMP Pressure (Pa),Light Intensity");
    dataFile.close();
    Serial.println("Header written to file");
  } else {
    Serial.println("Error creating requestMemory.csv");
    
  }
}

void requestMemory(int currentTime, float DHT11_temp, float humid, float press, String lightIntensity_string) {
  
  // Open the file in append mode and write sensor values
  File dataFile = SD.open("/requestMemory.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.print(currentTime);
    dataFile.print(",");
    dataFile.print(DHT11_temp);
    dataFile.print(",");
    dataFile.print(press);
    dataFile.print(",");
    dataFile.println(lightIntensity_string);
    dataFile.close();
    Serial.println("Data written to requestMemory.csv");

  } else {
    Serial.println("Error opening requestMemory.csv");
  }
}

void printRequestMemory(){
  File dataFile = SD.open("/requestMemory.csv", FILE_READ);
  if (dataFile) {
    Serial.println("Reading and printing file contents:");
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.println("Error opening requestMemory.csv for reading");
  }
}


void setupConstantMemory(){
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
  if (SD.remove("/constantMemory.csv")) {
      Serial.println("OK");
    } else {
      Serial.println("Error deleting constantMemory.csv");
    }

  File dataFile = SD.open("/constantMemory.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time (s),DHT Temperature (C),BMP Pressure (Pa),Light Intensity");
    dataFile.close();
    Serial.println("Header written to file");
  } else {
    Serial.println("Error creating constantMemory.csv");
    
  }
}

void constantMemory(int currentTime, float DHT11_temp, float humid, float press, String lightIntensity_string) {
  
  // Open the file in append mode and write sensor values
  File dataFile = SD.open("/constantMemory.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.print(currentTime);
    dataFile.print(",");
    dataFile.print(DHT11_temp);
    dataFile.print(",");
    dataFile.print(press);
    dataFile.print(",");
    dataFile.println(lightIntensity_string);
    dataFile.close();
    Serial.println("Data written to constantMemory.csv");

  } else {
    Serial.println("Error opening constantMemory.csv");
  }
}

void printConstantMemory(){
  File dataFile = SD.open("/constantMemory.csv", FILE_READ);
  if (dataFile) {
    Serial.println("Reading and printing file contents:");
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  } else {
    Serial.println("Error opening constantMemory.csv for reading");
  }
}