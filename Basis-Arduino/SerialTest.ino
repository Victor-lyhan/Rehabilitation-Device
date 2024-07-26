// serial transmiting
/*void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println("Hello World");
delay(200);
}
*/

//serial receiving
int blinkSecond = 1000;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
   if(Serial.available()) {
    String receive_data = Serial.readStringUntil('\n');
    blinkSecond = receive_data.toInt();
  }
  digitalWrite(9,LOW);
  delay(blinkSecond);
  digitalWrite(9,HIGH);
  delay(blinkSecond);  
  
}

