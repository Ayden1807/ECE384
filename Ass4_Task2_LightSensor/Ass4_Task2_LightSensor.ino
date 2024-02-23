#define output_Photoresistor 4

int PR_value;

void setup() {
  pinMode(output_Photoresistor, INPUT);

  Serial.begin(9600);
}

void loop() {
  // Read Analog to Digital Ouput of PR
  PR_value = analogRead(output_Photoresistor);

  Serial.print("Light Value: ");
  Serial.println(PR_value);
  delay(1000);
}
