void modeSelect() {
  lcd.setBacklight(233);
  aState = digitalRead(encoderCLK); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(encoderDATA) != aState) { 
       encoderPos ++;
       counter ++;
       } 
      else {
        encoderPos --;
        counter --;
        if(encoderPos == -1){
          encoderPos = 0;
        }
      }
    Serial.print("Encoder Position: ");
    Serial.println(encoderPos);

    // Serial.print(aLastState);
    // Serial.print("\t");
    // Serial.println(aState);

    Serial.print("Counter: ");
    Serial.println(counter);

    aLastState = aState;
    }
}