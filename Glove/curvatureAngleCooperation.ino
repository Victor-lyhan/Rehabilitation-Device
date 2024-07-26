//模式一 “seperate” 一根手指 两个关节 配合 一个先动 另一个在动
//模式二 “together” 一起动 另外一个继续懂

#include <math.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define magnValve1 23
#define magnValve2 24
#define magnValve9 31

#define airPump1 36
#define airPump2 37
#define airPumpEna 13

// #define airPressure0 A5
// #define airPressure1 A4

#define backCurvature A11
#define frontCurvature A9

///////


void airPumpControl(boolean state, int speed);
void magnValveControl(int state);

//Basic index
String input;
String mode;
int frontAngle = 0;
int backAngle = 0;
int frontCurvatureValue = 0;
int backCurvatureValue = 0;

//to split the input
// int spaceIndex = 0;
// int index = 0;

//for the second move after moving together
int secondState = 0;  // only for together mode
int biggerOne = 0x2; //电磁阀
int biggerCurvature = backCurvatureValue; //弯曲度
int biggerCur = A11; //弯曲度传感器
int smallerOne = 0x4;
int smallerCurvature = frontCurvatureValue;
int smallerCur = A9;

void setup() {
  // put your setup code here, to run once:
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //电磁阀
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve2, OUTPUT);
  pinMode(magnValve9, OUTPUT);
  //弯曲度
  pinMode(backCurvature, INPUT);
  pinMode(frontCurvature, INPUT);
  //气压传感器
  // pinMode(airPressure0, INPUT);
  // pinMode(airPressure1, INPUT);

  Serial.begin(9600);

  while (!Serial.available()) {
  }
  //判断前后气管 和 角度
  input = Serial.readStringUntil("/n");
  mode = input.substring(0, input.indexOf(": "));
  String aInput = input.substring(input.indexOf(":") + 2);
  backAngle = aInput.substring(0, aInput.indexOf(" ")).toInt();
  frontAngle = aInput.substring(aInput.indexOf(" ")).toInt();

  // 第一节转动角度 + 控制
  if (backAngle >= 70) {
    backCurvatureValue = ceil((backAngle - 30) * 1.65 + 269);
  } else {
    backCurvatureValue = ceil((backAngle - 30) * 1.65 + 269 - 15);
  }
  // 第二节转动角度 + 控制
  frontCurvatureValue = ceil((frontAngle - 30) * 1.13 + 262 + 10);
  //Serial.println(backCurvature)

  //"together" mode
  if (frontCurvatureValue > backCurvatureValue) {
    biggerCurvature = frontCurvatureValue;
    biggerOne = 0x4;
    biggerCur = A9;
    smallerCurvature = backCurvatureValue;
    smallerOne = 0x2;
    smallerCur = A11;
  }

  if (mode.indexOf("together") != -1) {
    secondState = biggerCurvature - smallerCurvature;
  }
}

boolean backDone = false;
boolean frontDone = false;
boolean togetherDone1 = false;
boolean togetherDone2 = false;

void loop() {
  if (mode.indexOf("seperate") != -1) { //"seperate" mode
    if (backDone == false) {
      airPumpControl(true, 150);
      magnValveControl(0x2);
      Serial.println(analogRead(backCurvature));
      //达到预期停止
      if (analogRead(backCurvature) >= backCurvatureValue) {
        airPumpControl(false, 110);
        magnValveControl(0x0);
        Serial.println(analogRead(backCurvature));
        backDone = true;
      }
    }
    if (backDone == true && frontDone == false) {
      airPumpControl(true, 150);
      magnValveControl(0x4);
      Serial.println(analogRead(frontCurvature));
      if (analogRead(frontCurvature) >= frontCurvatureValue) {
        airPumpControl(false, 110);
        magnValveControl(0x0);
        Serial.println(analogRead(frontCurvature));
        frontDone = true;
      }
    }
  } else if(mode.indexOf("together") != -1){ // "together" mode
      if (togetherDone1 == false) {
      airPumpControl(true, 150);
      magnValveControl(smallerOne + biggerOne);
      Serial.println(analogRead(backCurvature));
      //达到预期停止
      if (analogRead(backCurvature) >= smallerCurvature) {
        airPumpControl(false, 110);
        magnValveControl(0x0);
        Serial.println(analogRead(backCurvature));
        togetherDone1 = true;
      }
    }
    if (togetherDone1 == true && togetherDone2 == false) {
      airPumpControl(true, 155);
      magnValveControl(biggerOne);
      Serial.println(analogRead(biggerCur));
      if (analogRead(biggerCur) >= biggerCurvature) {
        airPumpControl(false, 110);
        magnValveControl(0x0);
        Serial.println(analogRead(biggerCur));
        togetherDone2 = true;
      }
    }
  }
  //放气
  if (Serial.available()) {
    String command = Serial.readStringUntil('/n');
    if (command.indexOf("release") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x206);
      delay(500);
    }
  }
  // Serial.println(analogRead(curvature0));
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
  //digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  digitalWrite(magnValve2, (state & 0x04));
  //digitalWrite(magnValve3, (state & 0x04));
  //digitalWrite(magnValve4, (state & 0x08));
  //digitalWrite(magnValve5, (state & 0x20));
  //digitalWrite(magnValve6, (state & 0x20));
  digitalWrite(magnValve9, (state & 0x200));
}
