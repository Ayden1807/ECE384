/* ===================================================================== *
 *                                                                       *
 * DISPLAY SYSTEM                                                        *
 *                                                                       *
 * ===================================================================== *
 * every "disp menu function" needs three functions 
 * - void LCDML_DISP_setup(func_name)    
 * - void LCDML_DISP_loop(func_name)     
 * - void LCDML_DISP_loop_end(func_name)
 *
 * EXAMPLE CODE:
    void LCDML_DISP_setup(..menu_func_name..) 
    {
      // setup
      // is called only if it is started

      // starts a trigger event for the loop function every 100 millisecounds
      LCDML_DISP_triggerMenu(100);  
    }
    
    void LCDML_DISP_loop(..menu_func_name..)
    { 
      // loop
      // is called when it is triggert
      // - with LCDML_DISP_triggerMenu( millisecounds ) 
      // - with every button status change

      // check if any button is presed (enter, up, down, left, right)
      if(LCDML_BUTTON_checkAny()) {         
        LCDML_DISP_funcend();
      } 
    }
    
    void LCDML_DISP_loop_end(..menu_func_name..)
    {
      // loop end
      // this functions is ever called when a DISP function is quit
      // you can here reset some global vars or do nothing  
    } 
 * ===================================================================== *
 */

void wifiSetup();
void apSetup();
void clearWifiMemory(const char* path2);
void wifiConnect();
void setupRequestMemory();
void requestMemory(int currentTime, float DHT11_temp, float humid, float press, String lightIntensity_string);
void printRequestMemory();
void setupConstantMemory();
void constantMemory(int currentTime, float DHT11_temp, float humid, float press, String lightIntensity_string);
void printConstantMemory();
void storeData(float DHT11_temp, float press, float humid, String lightIntensity_string);


DHT11 dht11(DHTPIN);
Adafruit_BMP280 bmp;
unsigned long lastButtonTime = 0;
unsigned long debounceDelay = 100;
int requestTime = 0;
int debugVariable = 0;

// *********************************************************************
// INFORMATION MODE
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_information){
  // setup function 
  lcd.setCursor(0, 0);
  lcd.print(F("Ayden Mosler"));
  lcd.setCursor(0, 1);
  lcd.print(F("Class of 2024"));
}

void LCDML_DISP_loop(LCDML_FUNC_information) 
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function
  
  if(LCDML_BUTTON_checkAny()) { // check if any button is presed (enter, up, down, left, right)
    // LCDML_DISP_funcend calls the loop_end function
    LCDML_DISP_funcend();
  } 
}

void LCDML_DISP_loop_end(LCDML_FUNC_information)
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing  
}  


// *********************************************************************
// REQUEST MODE
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_request_mode){
  // setup function   

  bmp.begin(BMP280_ADDRESS_ALT,  BMP280_CHIPID);
  lcd.setCursor(0, 0);
  lcd.print(F("REQUEST MODE"));
  lcd.setCursor(0, 1);
  lcd.print(F("ACTIVATED"));
  delay(5000);
  lcd.setBacklight(0);

  setupRequestMemory();
  // starts a trigger event for the loop function every 1 secounds
  LCDML_DISP_triggerMenu(1000);
  requestTime = 0;
}

void LCDML_DISP_loop(LCDML_FUNC_request_mode){ 

  float DHT11_temp = dht11.readTemperature();
  float humid = dht11.readHumidity();
  float press = bmp.readPressure() / 100.0;   //convert Pa to hPa for space saving
  // float BMP280_temp = bmp.readTemperature();
  int lightIntensity = analogRead(LIGHTPIN);
  String lightIntensity_string = "V.Dark";


unsigned long currentMillis = millis();

  if(currentMillis - lastButtonTime >= debounceDelay){
    if(LCDML_BUTTON_checkEnter()){
      LCDML_BUTTON_resetEnter();

      if(debugVariable == 1){
        Serial.print("DHT11 Temp: ");
        Serial.print(DHT11_temp);
        Serial.print("C\t DHT11 Humid: ");
        Serial.print(humid);
        Serial.print("%\t BMP280 Pressure: ");
        Serial.print(press);
        Serial.print("C\t Light Intensity: ");
        Serial.println(lightIntensity_string);
      }

      lcd.clear();
      lcd.setBacklight(255);
      Serial.println("BUTTON PRESSSED");
      lcd.setCursor(0, 0);
      lcd.print(DHT11_temp);
      lcd.write(223);
      lcd.print("C  ");
      lcd.print(humid);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print(press);
      lcd.print("hPa  ");
      if(lightIntensity > 2800){
        lcd.print("V.Dark");
      } else if(lightIntensity > 1000){
        lcd.print("Dark");
        lightIntensity_string.replace("V.Dark", "Dark");
      } else if(lightIntensity > 400){
        lcd.print("Bright");
        lightIntensity_string.replace("V.Dark","Bright");
      } else{
        lcd.print("V.Bright");
        lightIntensity_string.replace("V.Dark", "V.Bright");
      }

      delay(5000);
      lcd.setBacklight(0);
      lcd.clear();
      lastButtonTime = currentMillis;

      requestMemory(requestTime, DHT11_temp, humid, press, lightIntensity_string);
      requestTime++;
      storeData(DHT11_temp, press, humid, lightIntensity_string);

    }

  if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }   
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_request_mode) {
  lcd.setBacklight(255);
  printRequestMemory();
}

// *********************************************************************
// CONSTANT MODE

int constantTime = 0;
float tempSum = 0.0;
float humidSum = 0.0;
float pressSum = 0.0;
int lightSum = 0;
int sampleCount = 0;

unsigned long lastSampleTime = 0;
const long sampleInterval = 60000;
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_constant_mode){

  // setup function   
  bmp.begin(BMP280_ADDRESS_ALT,  BMP280_CHIPID);
  lcd.setCursor(0, 0);
  lcd.print(F("CONSTANT MODE"));
  lcd.setCursor(0, 1);
  lcd.print(F("ACTIVATED"));
  delay(5000);
  lcd.setBacklight(0);

  setupConstantMemory();
  // starts a trigger event for the loop function every 1 secounds
  LCDML_DISP_triggerMenu(1000);
  constantTime = 0;
  
  tempSum = 0.0;
  humidSum = 0.0;
  pressSum = 0.0;
  lightSum = 0;
  sampleCount = 0;
}

void LCDML_DISP_loop(LCDML_FUNC_constant_mode){ 
  unsigned long currentTime = millis();

  if (currentTime - lastSampleTime >= sampleInterval){
    lastSampleTime = currentTime;
  
    float DHT11_temp = dht11.readTemperature();
    float humid = dht11.readHumidity();
    float press = bmp.readPressure() / 100.0;   //convert Pa to hPa for space saving
    int lightIntensity = analogRead(LIGHTPIN);
    String lightIntensity_string = "V.Dark";

    if(debugVariable == 1){
      Serial.print("DHT11 Temp: ");
      Serial.print(DHT11_temp);
      Serial.print("C\t DHT11 Humid: ");
      Serial.print(humid);
      Serial.print("%\t BMP280 Pressure: ");
      Serial.print(press);
      Serial.print("C\t Light Intensity: ");
      Serial.println(lightIntensity_string);
    }

    lcd.clear();
    lcd.setBacklight(255);
    Serial.println("SENSORS SAMPLED");
    lcd.setCursor(0, 0);
    lcd.print(DHT11_temp);
    lcd.write(223);
    lcd.print("C  ");
    lcd.print(humid);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print(press);
    lcd.print("hPa  ");
    if(lightIntensity > 2800){
      lcd.print("V.Dark");
    } else if(lightIntensity > 1000){
      lcd.print("Dark");
      lightIntensity_string.replace("V.Dark", "Dark");
    } else if(lightIntensity > 400){
      lcd.print("Bright");
      lightIntensity_string.replace("V.Dark","Bright");
    } else{
      lcd.print("V.Bright");
      lightIntensity_string.replace("V.Dark", "V.Bright");
    }

    // Summing for avg
    tempSum += DHT11_temp;
    pressSum += press;
    humidSum += humid;
    lightSum += lightIntensity;
    sampleCount++;

    // Calc avg and save it to memory
    if(sampleCount == 10){
      float avgTemp = tempSum / 10;
      float avgPress = pressSum / 10;
      float avgHumid = humidSum / 10;
      int avgLight = lightSum / 10;

      tempSum = 0.0;
      pressSum = 0.0;
      humidSum = 0.0;
      lightSum = 0;
      sampleCount = 0;
      lightIntensity_string = "V.Dark";

      if(avgLight > 2800){
        lightIntensity_string = "V.Dark";
      } else if(lightIntensity > 1000){
        lightIntensity_string.replace("V.Dark", "Dark");
      } else if(lightIntensity > 400){
        lightIntensity_string.replace("V.Dark","Bright");
      } else{
        lightIntensity_string.replace("V.Dark", "V.Bright");
      }

      constantMemory(constantTime, avgTemp, avgHumid, avgPress, lightIntensity_string);
      constantTime++;

      storeData(DHT11_temp, press, humid, lightIntensity_string);
    }
  }

  if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }   
}

void LCDML_DISP_loop_end(LCDML_FUNC_constant_mode) {
  lcd.setBacklight(255);
  printConstantMemory();
}

//===================================================================== *
// DEBUG_MODE
//===================================================================== *
void LCDML_DISP_setup(LCDML_FUNC_debug_mode) {
  // setup
  lcd.clear();
  lcd.setBacklight(255);

  if (debugVariable == 0){
    debugVariable = 1;       //debug mode enabled
    lcd.setCursor(0, 0);
    lcd.print(F("DEBUG MODE"));
    lcd.setCursor(0, 1);
    lcd.print(F("ACTIVATED"));
    delay(5000);
  }else{
    debugVariable = 0;
    lcd.setCursor(0, 0);
    lcd.print(F("DEBUG MODE"));
    lcd.setCursor(0, 1);
    lcd.print(F("DISABLED"));
    delay(5000);
  }
  
  lcd.setBacklight(0);


  // starts a trigger event for the loop function every 1 secounds
  LCDML_DISP_triggerMenu(1000);
}

void LCDML_DISP_loop(LCDML_FUNC_debug_mode) {
}

void LCDML_DISP_loop_end(LCDML_FUNC_debug_mode) {
  lcd.setBacklight(255);
  lcd.clear();
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}

// *********************************************************************
// WIFI CONNECTION STATUS
// uint8_t g_button_value = 0; // button value counter (global variable)
void LCDML_DISP_setup(LCDML_FUNC_wifi_status)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);

  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0,1);
    lcd.print("IP:");
    lcd.print(WiFi.localIP());
  } else if (WiFi.getMode() == WIFI_AP){
    lcd.setCursor(0,0);
    lcd.print("AP Connected");
    lcd.setCursor(0,1);
    lcd.print("IP:");
    lcd.print(WiFi.softAPIP());
  } else{
    lcd.setCursor(0,0);
    lcd.print("Wifi Status:");
    lcd.setCursor(0,1);
    lcd.print("Offline");
  }
  // Reset Button Value
  // g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_wifi_status)
{
    if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_wifi_status) 
{
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
}

// *********************************************************************
// ACCESS POINT MODE
void LCDML_DISP_setup(LCDML_FUNC_ap_connect)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  apSetup();

  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0,0);
  lcd.print("Access Point");
  lcd.setCursor(0,1);
  lcd.print("Mode");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AP IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.softAPIP());

  // Reset Button Value
  // g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_ap_connect)
{
    if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_ap_connect) 
{
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
}

// *********************************************************************
// WIFI CONNECTION MODE
void LCDML_DISP_setup(LCDML_FUNC_wifi_connect)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0,0);
  lcd.print("Wifi Connect");
  lcd.setCursor(0,1);
  lcd.print("Mode");
  delay(5000);

  // wifiSetup();
  wifiConnect();

  // Reset Button Value
  // g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_wifi_connect)
{
    if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }
}
void LCDML_DISP_loop_end(LCDML_FUNC_wifi_connect) 
{
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
}

// *********************************************************************
// WIFI CLEAR MEMORY
void LCDML_DISP_setup(LCDML_FUNC_wifi_clear_memory)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  
  clearWifiMemory(filename);


  // Reset Button Value
  // g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_wifi_clear_memory)
{
    if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_wifi_clear_memory) 
{
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
}

// *********************************************************************
// DISCONNECT
void LCDML_DISP_setup(LCDML_FUNC_disconnect)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(0,0);
  lcd.print("Device is");
  lcd.setCursor(0,1);
  lcd.print("Offline");
  delay(5000);
  // Reset Button Value
  // g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_disconnect)
{
    if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_disconnect) 
{
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
}

// *********************************************************************
// REBOOT
void LCDML_DISP_setup(LCDML_FUNC_reboot)
// *********************************************************************
{ 
  Serial.println("ESP32 Rebooting...");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
  lcd.print("Device");
  lcd.setCursor(0, 1);
  lcd.print("Rebooting");
  delay(3000);
}

void LCDML_DISP_loop(LCDML_FUNC_reboot)
{
  ESP.restart();
}

void LCDML_DISP_loop_end(LCDML_FUNC_reboot) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}

// *********************************************************************
// FACTORY RESET
void LCDML_DISP_setup(LCDML_FUNC_factory_reset)
// *********************************************************************
{ 
  Serial.println("ESP32 Factory Reset...");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
  lcd.print("Attempting to");
  lcd.setCursor(0, 1);
  lcd.print("Factory RST");
  delay(3000);

  clearWifiMemory(filename);
  delay(2000);
  // ADD clearHistoricalMemory();
  // delay(2000);
  // ADD any other fields that need to be cleared...
  Serial.println("ESP32 Factory Reset Complete");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setBacklight(255);
  lcd.print("Factory RST");
  lcd.setCursor(0, 1);
  lcd.print("Complete");
  delay(3000);
}

void LCDML_DISP_loop(LCDML_FUNC_factory_reset)
{

}

void LCDML_DISP_loop_end(LCDML_FUNC_factory_reset) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}

// // *********************************************************************
// unsigned long g_initscreen_example_counter = 0;
// void LCDML_DISP_setup(LCDML_FUNC_initscreen)
// // *********************************************************************
// {
//   // setup function
//   LCDML_DISP_triggerMenu(1000); // set trigger for this function to 1000 millisecounds
//   lcd.print(F("InitScreen"));  // print first line to lcd display
//   g_initscreen_example_counter = 0; // reset or set example counter
// }

// void LCDML_DISP_loop(LCDML_FUNC_initscreen) 
// {
//   // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
//   // the quit button works in every DISP function without any checks; it starts the loop_end function 
//   g_initscreen_example_counter++; // count the example counter above
//   lcd.setCursor(0,1); // clear the secound line on lcd 
//   lcd.print(F("      "));
//   lcd.setCursor(0,1); // print new value to lcd
//   lcd.print(g_initscreen_example_counter);
  
//   g_lcdml_initscreen = millis(); // reset initscreen timer
  
//   if(LCDML_BUTTON_checkAny()) { // check if any button is pressed to left this function
//     LCDML_DISP_funcend(); // function end    
//   }   
// }

// void LCDML_DISP_loop_end(LCDML_FUNC_initscreen) 
// {  
//   // this functions is ever called when a DISP function is quit
//   // you can here reset some global vars or do nothing
//   LCDML.goRoot(); // go to root element (first element of this menu with id=0)
// }
