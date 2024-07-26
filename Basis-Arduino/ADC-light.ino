void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT);
}
void loop() {
  int i = analogRead(A1);
  Serial.println(i);
  delay(200);
}
