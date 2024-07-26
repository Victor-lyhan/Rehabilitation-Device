void setup() {
  // put your setup code here, to run once:
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(6,150);
  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
}
