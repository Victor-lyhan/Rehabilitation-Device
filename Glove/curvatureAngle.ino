#include <math.h>

#define magnValve0 52
#define magnValve1 53
#define magnValve5 49

#define airPump1 22
#define airPump2 23
#define airPumpEna 13

// #define airPressure0 A5
// #define airPressure1 A4

#define curvature A11
#define curvature1 A9

void airPumpControl(boolean state, int speed);
void magnValveControl(int state);

String input;
String whichOne;
int angleX = 0;
int angleCurvature = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //电磁阀
  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve5, OUTPUT);
  //气压传感器
  // pinMode(airPressure0, INPUT);
  // pinMode(airPressure1, INPUT);

  Serial.begin(9600);

  while (!Serial.available()) {
  }
  //判断前后气管 和 角度
  input = Serial.readStringUntil("/n");
  whichOne = input.substring(0, input.indexOf(" "));
  angleX = input.substring(input.indexOf(" ")).toInt();


  //开转
  if (whichOne.indexOf("front") != -1) {
    angleCurvature = ceil((angleX - 30) * 1.13 + 262 + 10);
    airPumpControl(true, 150);
    magnValveControl(0x2);
  } else {
    if (angleX >= 70) {
      angleCurvature = ceil((angleX - 30) * 1.65 + 269);
    } else {
      angleCurvature = ceil((angleX - 30) * 1.65 + 269 - 15);
    }
    airPumpControl(true, 150);
    magnValveControl(0x1);
  }
}

void loop() {
  if (whichOne.indexOf("front") != -1) {
    if (analogRead(curvature1) > angleCurvature) {
      airPumpControl(false, 110);
      magnValveControl(0x0);
      Serial.println(analogRead(curvature1));
    }
  } else {
    if (analogRead(curvature0) > angleCurvature) {
      airPumpControl(false, 110);
      magnValveControl(0x0);
      Serial.println(analogRead(curvature0));
    }
  }
  //放气
  if (Serial.available()) {
    String command = Serial.readStringUntil('/n');
    if (command.indexOf("release") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x23);
      delay(500);
    }
  }
  Serial.println(analogRead(curvature0));
}



void airPumpControl(boolean state, int speed) {
  if (state == true) {
    digitalWrite(airPump1, HIGH);
    digitalWrite(airPump2, LOW);
    analogWrite(airPumpEna, speed);
  } else {
    digitalWrite(airPump1, LOW);
    digitalWrite(airPump2, LOW);
  }
}

void magnValveControl(int state) {
  digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  // digitalWrite(magnValve3, (state & 0x04));
  // digitalWrite(magnValve4, (state & 0x08));
  digitalWrite(magnValve5, (state & 0x20));
  // digitalWrite(magnValve6, (state & 0x20));
}

// String inputSplit[] (String input) {
//   String data[2];
//   String aInput = input;
//   String whichOne = aInput.substring(0, indexOf(" "));
//   data[0] = whichOne;
//   String angle = aInput.substring(indexOf(" "));
//   data[1] = angle;
//   return data;
// }
