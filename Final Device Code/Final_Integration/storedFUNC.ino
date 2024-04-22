// Stored values used for websites
float storedTemp = 0.0;
float storedPress = 0.0;
float storedHumid = 0.0;
String storedLight = "";

void storeData(float temp, float press, float humid, String lightIntensity_string){
  storedTemp = temp;
  storedPress = press;
  storedHumid = humid;
  storedLight = lightIntensity_string;
}

float returnTemp(){
  return storedTemp;
}

float returnPress(){
  return storedPress;
}

float returnHumid(){
  return storedHumid;
}

String returnLight(){
  return storedLight;
}