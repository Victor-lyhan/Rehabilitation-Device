#define Green 9
#define Blue 10
#define Red 11
int mode = 0;

void onChange() {
  mode++;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), onChange, CHANGE);
}

void lightControl(int RedColor, int BlueColor, int GreenColor) {
  analogWrite(Green, 255 - GreenColor);
  analogWrite(Blue, 255 - BlueColor);
  analogWrite(Red, 255 - RedColor);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mode % 2 == 0) {
    lightControl(25, 12, 20);
  } else {
    lightControl(25, 12, 20);
    delay(100);
    lightControl(0, 0, 0);
    delay(100);
  }
}