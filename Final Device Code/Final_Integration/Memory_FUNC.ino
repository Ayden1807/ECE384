void setupRequestMemory(){
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.print("Card Mount Failed\t");
    return;
  }
  Serial.print("OK\t");

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.print("MMC\t");
  } else if (cardType == CARD_SD) {
    Serial.print("SDSC\t");
  } else if (cardType == CARD_SDHC) {
    Serial.print("SDHC\t");
  } else {
    Serial.print("UNKNOWN\t");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.println();

  // Create CSV file and write header
  if (SD.remove("/requestMemory.csv")) {
      Serial.println("Deleted requestMemory.csv for new session");
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
    Serial.print("Card Mount Failed\t");
    return;
  }
  Serial.print("OK\t");

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.print("No SD card attached\t");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.print("MMC\t");
  } else if (cardType == CARD_SD) {
    Serial.print("SDSC\t");
  } else if (cardType == CARD_SDHC) {
    Serial.print("SDHC\t");
  } else {
    Serial.print("UNKNOWN\t");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.println();

  // Create CSV file and write header
  if (SD.remove("/constantMemory.csv")) {
      Serial.println("Deleted constantMemory.csv for new session");
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
    Serial.println("Average data written to constantMemory.csv");

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

void clearHistoricalMemory(){
  lcd.clear();
  if (SD.remove("/constantMemory.csv")) {
      Serial.println("Deleted constantMemory.csv");
      lcd.setCursor(0,0);
      lcd.print("Local Constant");
      lcd.setCursor(0,1);
      lcd.print("Mem Cleared");
    } else {
      Serial.println("Error deleting constantMemory.csv");
      lcd.setCursor(0,0);
      lcd.print("ErrorClearing");
      lcd.setCursor(0,1);
      lcd.print("Constant Mem");
    }
  delay(3000);
  File dataFile = SD.open("/constantMemory.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time (s),DHT Temperature (C),BMP Pressure (Pa),Light Intensity");
    dataFile.close();
    Serial.println("Header written to file");
  } else {
    Serial.println("Error creating constantMemory.csv");
  }

  lcd.clear();
  if (SD.remove("/requestMemory.csv")) {
      Serial.println("Deleted requestMemory.csv");
      lcd.setCursor(0,0);
      lcd.print("Local Request");
      lcd.setCursor(0,1);
      lcd.print("Mem Cleared");
    } else {
      Serial.println("Error deleting requestMemory.csv");
      lcd.setCursor(0,0);
      lcd.print("ErrorClearing");
      lcd.setCursor(0,1);
      lcd.print("Constant Mem");
    }
  delay(3000);
  dataFile = SD.open("/requestMemory.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time (s),DHT Temperature (C),BMP Pressure (Pa),Light Intensity");
    dataFile.close();
    Serial.println("Header written to file");
  } else {
    Serial.println("Error creating constantMemory.csv");
  }


}