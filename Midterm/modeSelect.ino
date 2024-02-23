void modeSelect() {
  lcd.setBacklight(255);
  aState = digitalRead(encoderCLK);

  // If the previous and the current state of the outputA are different, that means a Pulse has occurred
  if (aState != aLastState) {
    // If the outputB state is different from the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(encoderDATA) != aState) {
      encoderPos++;
      counter++;
      if(encoderPos > 5){
        encoderPos = 0;
      }
    } else {
      encoderPos--;
      counter--;
      if (encoderPos < 0) {
        encoderPos = 5;
      }
    }

    Serial.print("Encoder Position: ");
    Serial.println(encoderPos);

    // Serial.print("Counter: ");
    // Serial.println(counter);

    aLastState = aState;
  }

  // Check for button press
  if (digitalRead(encoderSW) == LOW) {
    desiredMode = encoderPos;
    encoderPos = 0;
    Serial.println("Button Pressed!");
    // Additional actions you may want to perform when the button is pressed
  }
}