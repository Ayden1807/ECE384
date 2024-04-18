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

// *********************************************************************
// INFORMATION MODE
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_information){
  // setup function 
  lcd.setCursor(0, 0);
  lcd.print(F("Ayden Mosler"));
  lcd.setCursor(0, 1);
  lcd.print(F("Class of 2024"));
  // lcd.setCursor(0, 2);
  // lcd.print(F("Taste druecken oder"));
  // lcd.setCursor(0, 3);
  // lcd.print(F("Back Taste verwenden"));
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
// NORMAL MODE
DHT11 dht11(DHTPIN);
Adafruit_BMP280 bmp;
unsigned long lastButtonTime = 0;
unsigned long debounceDelay = 100;
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_normal_mode){
  // setup function   
  bmp.begin(BMP280_ADDRESS_ALT,  BMP280_CHIPID);
  lcd.setCursor(0, 0);
  lcd.print(F("NORMAL MODE"));
  lcd.setCursor(0, 1);
  lcd.print(F("ACTIVATED"));
  delay(5000);
  lcd.setBacklight(0);

  // starts a trigger event for the loop function every 1 secounds
  LCDML_DISP_triggerMenu(1000);
}

void LCDML_DISP_loop(LCDML_FUNC_normal_mode){ 
  float DHT11_temp = dht11.readTemperature();
  float humid = dht11.readHumidity();
  float press = bmp.readPressure() / 100.0;   //convert Pa to hPa for space saving
  float BMP280_temp = bmp.readTemperature();
  int lightIntensity = analogRead(LIGHTPIN);

unsigned long currentMillis = millis();

  if(currentMillis - lastButtonTime >= debounceDelay){
    if(LCDML_BUTTON_checkEnter()){
      LCDML_BUTTON_resetEnter();
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
      } else if(lightIntensity > 400){
        lcd.print("Bright");
      } else{
        lcd.print("V.Bright");
      }

      delay(5000);
      lcd.setBacklight(0);
      lcd.clear();
      lastButtonTime = currentMillis;
    }

  

  if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }   
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_normal_mode) {
  lcd.setBacklight(255);
}


//===================================================================== *
// DEBUG_MODE
// DHT11 dht11(DHTPIN);
// Adafruit_BMP280 bmp;
// unsigned long lastButtonTime = 0;
// unsigned long debounceDelay = 100;
//===================================================================== *
void LCDML_DISP_setup(LCDML_FUNC_debug_mode) {
  // setup
  bmp.begin(BMP280_ADDRESS_ALT,  BMP280_CHIPID);
  lcd.setCursor(0, 0);
  lcd.print(F("DEBUG MODE"));
  lcd.setCursor(0, 1);
  lcd.print(F("ACTIVATED"));
  delay(5000);
  lcd.setBacklight(0);


  // starts a trigger event for the loop function every 1 secounds
  LCDML_DISP_triggerMenu(1000);
}

void LCDML_DISP_loop(LCDML_FUNC_debug_mode) {
  float DHT11_temp = dht11.readTemperature();
  float humid = dht11.readHumidity();
  float press = bmp.readPressure() / 100.0;   //convert Pa to hPa for space saving
  float BMP280_temp = bmp.readTemperature();
  int lightIntensity = analogRead(LIGHTPIN);

  Serial.print("DHT11 Temp: ");
  Serial.print(DHT11_temp);
  Serial.print("C\t DHT11 Humid: ");
  Serial.print(humid);
  Serial.print("%\t BMP280 Pressure: ");
  Serial.print(press);
  Serial.print("Pa\t BMP280 Temp: ");
  Serial.print(BMP280_temp);
  Serial.print("C\t Light Intensity: ");
  Serial.println(lightIntensity);
  
  // loop
  // is called when it is triggert
  // - with LCDML_DISP_triggerMenu( millisecounds )
  // - with every button status change

  unsigned long currentMillis = millis();

  if(currentMillis - lastButtonTime >= debounceDelay){
    if(LCDML_BUTTON_checkEnter()){
      LCDML_BUTTON_resetEnter();
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
      } else if(lightIntensity > 400){
        lcd.print("Bright");
      } else{
        lcd.print("V.Bright");
      }

      delay(5000);
      lcd.setBacklight(0);
      lcd.clear();
      lastButtonTime = currentMillis;
    }
  }
  // check if any button is presed (up or down)
  if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
    LCDML_BUTTON_resetLeft();
    LCDML_BUTTON_resetRight();
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_debug_mode) {
  lcd.setBacklight(255);
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}

// *********************************************************************
uint8_t g_button_value = 0; // button value counter (global variable)
void LCDML_DISP_setup(LCDML_FUNC_p2)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("press left or up"));  
  lcd.setCursor(0, 1);
  lcd.print(F("count: 0 of 3"));
  // Reset Button Value
  g_button_value = 0; 
}

void LCDML_DISP_loop(LCDML_FUNC_p2)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
  
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkLeft() || LCDML_BUTTON_checkUp()) // check if button left is pressed
    {
      LCDML_BUTTON_resetLeft(); // reset the left button
      LCDML_BUTTON_resetUp(); // reset the left button
      g_button_value++;
      
      // update lcd content
      lcd.setCursor(7, 1); // set cursor   
      lcd.print(g_button_value); // print change content
    }    
  }
  
  // check if button count is three
  if (g_button_value >= 3) {
    // end function for callback
    LCDML_DISP_funcend();   
  } 
}

void LCDML_DISP_loop_end(LCDML_FUNC_p2) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}



// *********************************************************************
unsigned long g_initscreen_example_counter = 0;
void LCDML_DISP_setup(LCDML_FUNC_initscreen)
// *********************************************************************
{
  // setup function
  LCDML_DISP_triggerMenu(1000); // set trigger for this function to 1000 millisecounds
  lcd.print(F("InitScreen"));  // print first line to lcd display
  g_initscreen_example_counter = 0; // reset or set example counter
}

void LCDML_DISP_loop(LCDML_FUNC_initscreen) 
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
  g_initscreen_example_counter++; // count the example counter above
  lcd.setCursor(0,1); // clear the secound line on lcd 
  lcd.print(F("      "));
  lcd.setCursor(0,1); // print new value to lcd
  lcd.print(g_initscreen_example_counter);
  
  g_lcdml_initscreen = millis(); // reset initscreen timer
  
  if(LCDML_BUTTON_checkAny()) { // check if any button is pressed to left this function
    LCDML_DISP_funcend(); // function end    
  }   
}

void LCDML_DISP_loop_end(LCDML_FUNC_initscreen) 
{  
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
  LCDML.goRoot(); // go to root element (first element of this menu with id=0)
}
