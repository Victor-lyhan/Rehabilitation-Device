#define Green 9
#define Blue 10
#define Red 11

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void lightControl(int RedColor, int BlueColor, int GreenColor) {
  analogWrite(Green, 255 - GreenColor);
  analogWrite(Blue, 255 - BlueColor);
  analogWrite(Red, 255 - RedColor);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(2) == HIGH) {
    lightControl(50, 100, 200);
  } else {
    lightControl(0, 0, 0);
  }
}
