// ============================================================                                                            
// Example:     LCDML: initscreen                          
// ============================================================
// Autor:       Nils Feldkämper
// Last update: 08.01.2017
// License:     MIT                                     
// ============================================================ 

  // DEBUG TURN ON/OFF
  // #define DEBUG_WIFI

  // include libs
  #include <ESP.h>
  #include <WiFi.h>
  #include <WebServer.h>
  #include <SPIFFS.h>
  #include <LCDMenuLib.h>
  #include <LiquidCrystal_PCF8574.h>
  #include <DHT11.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BMP280.h>
  #include <FS.h>
  #include <SD.h>
  #include <SPI.h>
  
  // *******************************************************************
  // MY PINS
  #define DHTPIN 16
  #define LIGHTPIN 36       //Change this when I see fit
  #define RST 0

  // *******************************************************************
  // Iot Information
  const char *ssid = "ESP32-Access-Point";
  const char *password = "123456789";

  const char *filename = "/wifi.txt";
  WebServer server(80);

  // *******************************************************************

  // lib config
  #define _LCDML_DISP_cfg_button_press_time          200    // button press time in ms
  #define _LCDML_DISP_cfg_initscreen_time            10000  // enable initscreen time
  #define _LCDML_DISP_cfg_scrollbar                  1      // enable a scrollbar
  #define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol 

// ********************************************************************* 
// LCDML TYPE SELECT
// *********************************************************************
  // settings for lcd 
  #define _LCDML_DISP_cols             16
  #define _LCDML_DISP_rows             2  

  // lcd object
  // liquid crystal needs (rs, e, dat4, dat5, dat6, dat7)
  LiquidCrystal_PCF8574 lcd(0x27);
  DHT11 dht(DHTPIN);
  
  const uint8_t scroll_bar[5][8] = {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1 
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  }; 

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  // create menu
  // menu element count - last element id
  // this value must be the same as the last menu element
  #define _LCDML_DISP_cnt    13
  
  // LCDML_root        => layer 0 
  // LCDML_root_X      => layer 1 
  // LCDML_root_X_X    => layer 2 
  // LCDML_root_X_X_X  => layer 3 
  // LCDML_root_... 	 => layer ... 
  
  // LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
  LCDML_DISP_init(_LCDML_DISP_cnt);
  LCDML_DISP_add      (0  , _LCDML_G1  , LCDML_root        , 1  , "Creator Info"       , LCDML_FUNC_information);
  LCDML_DISP_add      (1  , _LCDML_G1  , LCDML_root        , 2  , "Request Mode"       , LCDML_FUNC_request_mode);
  LCDML_DISP_add      (2  , _LCDML_G1  , LCDML_root        , 3  , "Constant Mode"      , LCDML_FUNC_constant_mode);
  LCDML_DISP_add      (3  , _LCDML_G1  , LCDML_root        , 4  , "Debug Mode"         , LCDML_FUNC_debug_mode);
  LCDML_DISP_add      (4  , _LCDML_G1  , LCDML_root        , 5  , "WiFi Settings"      , LCDML_FUNC);
  LCDML_DISP_add      (5  , _LCDML_G1  , LCDML_root_5      , 1  , "Conn. Status"       , LCDML_FUNC_wifi_status);
  LCDML_DISP_add      (6  , _LCDML_G1  , LCDML_root_5      , 2  , "Conn. to AP"        , LCDML_FUNC_ap_connect);
  LCDML_DISP_add      (7  , _LCDML_G1  , LCDML_root_5      , 3  , "Conn. to WiFi"      , LCDML_FUNC_wifi_connect);
  LCDML_DISP_add      (8  , _LCDML_G1  , LCDML_root_5      , 4  , "Delete Saved"       , LCDML_FUNC_wifi_clear_memory);
  LCDML_DISP_add      (9  , _LCDML_G1  , LCDML_root_5      , 5  , "Disconnect"         , LCDML_FUNC_disconnect);
  LCDML_DISP_add      (10 , _LCDML_G1  , LCDML_root        , 6  , "WS Settings"        , LCDML_FUNC);
  LCDML_DISP_add      (11 , _LCDML_G1  , LCDML_root_6      , 1  , "CLR LocalHist"      , LCDML_FUNC_clear_history);
  LCDML_DISP_add      (12 , _LCDML_G1  , LCDML_root_6      , 2  , "Reboot"             , LCDML_FUNC_reboot);
  LCDML_DISP_add      (13 , _LCDML_G1  , LCDML_root_6      , 3  , "Factory Reset"      , LCDML_FUNC_factory_reset);

  // LCDML_DISP_add      (7  , _LCDML_G1  , LCDML_root_4_1_2  , 1  , "Warm"               , LCDML_FUNC);
  // LCDML_DISP_add      (8  , _LCDML_G1  , LCDML_root_4_1_2  , 2  , "Long"               , LCDML_FUNC);
  // LCDML_DISP_add      (9  , _LCDML_G1  , LCDML_root_4      , 2  , "Program 2"          , LCDML_FUNC_p2);
  // LCDML_DISP_add      (10 , _LCDML_G7  , LCDML_root        , 5  , "Program 2"          , LCDML_FUNC_initscreen); // in g7 => hidden
  LCDML_DISP_createMenu(_LCDML_DISP_cnt);



// ********************************************************************* 
// LCDML BACKEND (core of the menu, do not change here anything yet)
// ********************************************************************* 
  // define backend function  
  #define _LCDML_BACK_cnt    1  // last backend function id
  
  LCDML_BACK_init(_LCDML_BACK_cnt);
  LCDML_BACK_new_timebased_dynamic (0  , ( 20UL )         , _LCDML_start  , LCDML_BACKEND_control);
  LCDML_BACK_new_timebased_dynamic (1  , ( 1000UL )       , _LCDML_stop   , LCDML_BACKEND_menu);
  LCDML_BACK_create();

  void wifiSetup();
  void wifiFactoryReset(const char* filename);

// *********************************************************************
// SETUP
// *********************************************************************
  void setup()
  {  
    Serial.begin(115200);                // start serial   
    
    wifiSetup();

    // serial init; only be needed if serial control is used    
 
    Serial.println(F(_LCDML_VERSION)); // only for examples

    // LCD Begin
    lcd.begin(_LCDML_DISP_cols,_LCDML_DISP_rows);
    lcd.setBacklight(255);  
    // set special chars for scrollbar
    lcd.createChar(0, (uint8_t*)scroll_bar[0]);
    lcd.createChar(1, (uint8_t*)scroll_bar[1]);
    lcd.createChar(2, (uint8_t*)scroll_bar[2]);
    lcd.createChar(3, (uint8_t*)scroll_bar[3]);
    lcd.createChar(4, (uint8_t*)scroll_bar[4]);
  
    // Enable all items with _LCDML_G1
    LCDML_DISP_groupEnable(_LCDML_G1); // enable group 1
    
    // Enable menu rollover if needed
    //LCDML.enRollover();
  
    // LCDMenu Setup
    LCDML_setup(_LCDML_BACK_cnt);  
  }

// *********************************************************************
// LOOP
// *********************************************************************
  void loop()
  { 
    // example for init screen (This will turn the screen back to a constant display after not being used for a few seconds)  

    // if((millis() - g_lcdml_initscreen) >= _LCDML_DISP_cfg_initscreen_time) {
    //   g_lcdml_initscreen = millis(); // reset init screen time
    //   LCDML_DISP_jumpToFunc(LCDML_FUNC_initscreen); // jump to initscreen     
    // }  
    
    // this function must called here, do not delete it
    LCDML_run(_LCDML_priority); 
    
    server.handleClient();

  if(digitalRead(RST) == LOW){
    Serial.println("RST Button Pressed");
    wifiFactoryReset(filename);
    delay(1000);
  }
  }


// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib.h
# endif
# if(_LCDML_DISP_cols > _LCDML_DISP_cfg_max_string_length)
# error change value of _LCDML_DISP_cfg_max_string_length in LCDMenuLib.h
# endif
