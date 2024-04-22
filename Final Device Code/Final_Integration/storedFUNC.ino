// Stored values used for websites
float storedTemp = 0.0;
float storedPress = 0.0;
float storedHumid = 0.0;
String storedLight = "";

void storeTemp(float temp){
  storedTemp = temp;
}

float returnTemp(){
  return storedTemp;
}