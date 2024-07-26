#define Green 9
#define Blue 10
#define Red 11

void setup() {
  // put your setup code here, to run once:
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(Red, OUTPUT);
}

void lightControl(int RedColor, int BlueColor, int GreenColor) {
  analogWrite(Green, 255 - GreenColor);
  analogWrite(Blue, 255 - BlueColor);
  analogWrite(Red, 255 - RedColor);
}
void loop() {
  lightControl(100, 200, 100);
  delay(500);
  lightControl(0, 0, 0);
  delay(500);
  lightControl(100, 200, 100);
  delay(500);
  lightControl(0, 0, 0);
  delay(500);
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 255; i++) {
      lightControl(i, i, i);
      delay(10);
    }
    for (int i = 255; i >= 0; i--) {
      lightControl(i, i, i);
      delay(10);
    }
  }
  while (1)
    ;
}
