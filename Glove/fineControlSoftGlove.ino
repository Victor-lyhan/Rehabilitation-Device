#include <math.h>
//气泵
#define airPump1 22
#define airPump2 23
#define airPumpEna 13
//电磁阀
#define magnValve0 52
#define magnValve1 53
#define magnValve2 50
#define magnValve3 51
#define magnValve4 48
#define magnValve5 49

#define curvature A0
#define pressure A1
#define airPressure A2


//statements
void airPumpControl(boolean state, int speed);
void magnValveControl(int state);

//转动角度
String angle;
int angleX = 0;
int angleCurvature = 0;

void setup() {
  //气泵
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //电磁阀
  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve2, OUTPUT);
  pinMode(magnValve3, OUTPUT);
  pinMode(magnValve4, OUTPUT);
  pinMode(magnValve5, OUTPUT);
  //弯曲度传感器
  pinMode(curvature, INPUT);
  //压力传感器
  pinMode(pressure, INPUT);
  //气压传感器
  pinMode(airPressure, INPUT);
  Serial.begin(9600);
  //读多少度
  while (!Serial.available()) {
  }
  angle = Serial.readStringUntil('/n');
  angleX = angle.toInt();
  angleCurvature = ceil(angleX * 0.8 + 95);
  //开转
  airPumpControl(true, angleCurvature+5);
  magnValveControl(0x20);
}

void loop() {
  // put your main code here, to run repeatedly:

  // magnValveControl(0x30);
  //print index
  Serial.print("curvature: ");
  Serial.print(analogRead(curvature));
  Serial.print(", pressure: ");
  Serial.print(analogRead(pressure));

  Serial.print(", airPressure: ");
  Serial.println(analogRead(airPressure));
  //delay(100);

  //判断是否达到预设转动角度
  if (analogRead(curvature) > angleCurvature) {
    airPumpControl(false, 110);
    magnValveControl(0x0);
  }

  //放气
  if (Serial.available()) {
    String command = Serial.readStringUntil('/n');
    if (command.indexOf("release") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x30);
      delay(1000);
      magnValveControl(0x20);
    }
  }
}

//气泵
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

//电磁阀
void magnValveControl(int state) {
  digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  digitalWrite(magnValve2, (state & 0x04));
  digitalWrite(magnValve3, (state & 0x08));
  digitalWrite(magnValve4, (state & 0x10));
  digitalWrite(magnValve5, (state & 0x20));
}

/*
  for (int i = 0; i < 3; i++) {
    // 
    for (int j = 0; j < 6; j++) {
      magnValveControl(32 >> j);
      delay(500);
    }
  }
  */
