 #define output_encoderCLK 18
 #define output_encoderDATA 19
 #define output_encoderSW 13

 int counter = 0; 
 int aState;
 int aLastState;  
 int SW_state;

 void setup() { 
   pinMode (output_encoderCLK,INPUT);
   pinMode (output_encoderDATA,INPUT);
   pinMode (output_encoderSW, INPUT);
   
   Serial.begin (9600);
   // Reads the initial state of the outputA
   aLastState = digitalRead(output_encoderCLK);   
 } 

 void loop() { 
   aState = digitalRead(output_encoderCLK); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(output_encoderDATA) != aState) { 
       counter ++;
     } else {
       counter --;
     }
    //Serial.print("Position:");
     Serial.println(counter);
   } 
   SW_state = digitalRead(output_encoderSW);

   if (SW_state == LOW) {
    counter = 0;
    Serial.println("Counter Reset");
   }
   aLastState = aState; // Updates the previous state of the outputA with the current state
 }