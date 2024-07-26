//模式一 “seperate” 一根手指 两个关节 配合 一个先动 另一个在动
//模式二 “together” 一起动 另外一个继续懂

#include <math.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define magnValve0 22  //负压泵
#define magnValve1 23  //大拇指
#define magnValve2 24  //食指
#define magnValve3 25
#define magnValve4 26  //中指
#define magnValve5 27
#define magnValve6 28  //无名指
#define magnValve7 29
#define magnValve8 30  //小拇指
#define magnValve9 31
// air pump
#define airPump1_1 38
#define airPump1_2 39
#define airPumpEna1 13
#define airPump2_1 48
#define airPump2_2 49
#define airPumpEna2 9
//airPressure sensor
#define airPressure8 A0
#define airPressure7 A1
#define airPressure6 A2
#define airPressure5 A3  // +35
#define airPressure4 A4  // +35
#define airPressure3 A5
#define airPressure2 A6
#define airPressure1 A7
//curvature sensor
// #define finger0Curvature A1
// #define finger1backCurvature A2
// #define finger1frontCurvature A3
// #define finger2backCurvature A4
// #define finger2frontCurvature A5
// #define finger3backCurvature A6
// #define finger3frontCurvature A7
// #define finger4Curvature A8



//全局变量

Adafruit_ADS1X15 ads[4];
//IIC channel selection
void selectBoard(byte board);
int sensorData[4][4];  //data by board 前三排中间三根手指食指中指无名指 第四排前两项大拇指后两项小拇指
int magnBackControlCriteria[5] = { sensorData[3][0], sensorData[0][0], sensorData[1][0], sensorData[2][0], sensorData[3][2] };
int magnFrontControlCriteria[3] = { sensorData[0][2], sensorData[1][2], sensorData[2][2] };

void updateSensorData();
//airPump
void airPumpControl(boolean state, int speed);
//magnValve
void magnValveControl(int state);
//findBaseCurvature
int findBaseCurvature(int a, int b, int c, int d, int e);

//General
String input;
String handMode;  // "hand"/"fingers"
String airMode;   // "seperate"/"together"
int frontAngle = 0;
int backAngle = 0;
int backBaseCurvature = 0;
int frontBaseCurvature = 0;

// front and back curvature for five fingers
//finger0 大拇指
int finger0SpecificAngle = 0;
int finger0CurvatureValue = 0;
//finger1 食指
int finger1FrontCurvatureValue = 0;
int finger1BackCurvatureValue = 0;
//finger2 中指
int finger2FrontCurvatureValue = 0;
int finger2BackCurvatureValue = 0;
//finger3 无名指
int finger3FrontCurvatureValue = 0;
int finger3BackCurvatureValue = 0;
//finger4 小拇指
int finger4SpecificAngle = 0;
int finger4CurvatureValue = 0;

int fingerBackCurvature[5] = { finger0CurvatureValue, finger1BackCurvatureValue, finger2BackCurvatureValue, finger3BackCurvatureValue, finger4CurvatureValue };
int fingerFrontCurvature[3] = { finger1FrontCurvatureValue, finger2FrontCurvatureValue, finger3FrontCurvatureValue };

int currentMagnState = 0x00;

// //SecondState for five fingers “together”
// //finger0
// //finger1
// int finger1BiggerOne = 0x2; //magnetic valve
// int finger1BiggerCur = A11; //curvature sensor
// int finger1SmallerOne = 0x4;
// int finger1SmallerCur = A9;
// //finger2
// int finger2BiggerOne = 0x2;
// int finger2BiggerCur = A11;
// int finger2SmallerOne = 0x4;
// int finger2SmallerCur = A9;
// //finger3
// int finger3BiggerOne = 0x2;
// int finger3BiggerCur = A11;
// int finger3SmallerOne = 0x4;
// int finger3SmallerCur = A9;
// //finger4

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  for (int i = 0; i < 4; i++) {
    ads[i].begin();
    ads[i].setGain(GAIN_ONE);
  }
  pinMode(airPump1_1, OUTPUT);
  pinMode(airPump1_2, OUTPUT);
  pinMode(airPumpEna1, OUTPUT);
  //magnValve
  //  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve2, OUTPUT);
  pinMode(magnValve3, OUTPUT);
  pinMode(magnValve4, OUTPUT);
  pinMode(magnValve5, OUTPUT);
  pinMode(magnValve6, OUTPUT);
  pinMode(magnValve7, OUTPUT);
  pinMode(magnValve8, OUTPUT);
  //  pinMode(magnValve9, OUTPUT);
  //弯曲度
  // pinMode(finger0Curvature, INPUT);
  // pinMode(finger1backCurvature, INPUT);
  // pinMode(finger1backCurvature, INPUT);
  // pinMode(finger2backCurvature, INPUT);
  // pinMode(finger2backCurvature, INPUT);
  // pinMode(finger3backCurvature, INPUT);
  // pinMode(finger3backCurvature, INPUT);
  // pinMode(finger4Curvature, INPUT);

  //气压传感器
  // pinMode(airPressure0, INPUT);
  // pinMode(airPressure1, INPUT);

  magnValveControl(0x1fe);
  delay(1000);
  Serial.begin(115200);

  // while (!Serial.available()) {
  // }
  // input format "hand/finger: seperate/together 50 40 50(thumb) 60(little finger)"
  //判断前后气管 和 角度
  // input = Serial.readStringUntil("/n");
  // handMode = input.substring(0, input.indexOf(": "));
  // String aInput = input.substring(input.indexOf(":") + 2);  //without handmode
  // airMode = aInput.substring(0, aInput.indexOf(" "));
  // String bInput = aInput.substring(aInput.indexOf(" ") + 1);
  // backAngle = bInput.substring(0, bInput.indexOf(" ")).toInt();
  // String cInput = bInput.substring(bInput.indexOf(" ") + 1);
  // frontAngle = bInput.substring(bInput.indexOf(" ")).toInt();
  // String dInput = cInput.substring(cInput.indexOf(" ") + 1);
  // finger0SpecificAngle = dInput.substring(0, dInput.indexOf(" ")).toInt();
  // finger4SpecificAngle = dInput.substring(dInput.indexOf(" ")).toInt();
  handMode = "hand";
  airMode = "seperate";
  //thumb
  finger0CurvatureValue = 12000;
  //little finger
  finger4CurvatureValue = 8888;
  // 第一节转动弯曲度
  //backCurvatureValue for three middle fingers
  // need change
  finger1BackCurvatureValue = 12000;
  finger2BackCurvatureValue = 12000;
  finger3BackCurvatureValue = 11300;

  // 第二节转动弯曲度
  //frontCurvatureValue for three middle fingers
  finger1FrontCurvatureValue = 10000;
  finger2FrontCurvatureValue = 12500;
  finger3FrontCurvatureValue = 13200;

  //base
  backBaseCurvature = findBaseCurvature(finger0CurvatureValue, finger1BackCurvatureValue, finger2BackCurvatureValue, finger3BackCurvatureValue, finger4CurvatureValue);
  frontBaseCurvature = findBaseCurvature2(finger1FrontCurvatureValue, finger2FrontCurvatureValue, finger3FrontCurvatureValue);


  //"together" mode
  // if (frontCurvatureValue > backCurvatureValue) {
  //   biggerCurvature = frontCurvatureValue;
  //   biggerOne = 0x4;
  //   biggerCur = A9;
  //   smallerCurvature = backCurvatureValue;
  //   smallerOne = 0x2;
  //   smallerCur = A11;
  // }

  // if (airMode.indexOf("together") != -1) {
  //   secondState = biggerCurvature - smallerCurvature;
  // }
}

//"hand" "seperate"
boolean finger0Done = false;
boolean finger1BackDone = false;
boolean finger1FrontDone = false;
boolean finger2BackDone = false;
boolean finger2FrontDone = false;
boolean finger3BackDone = false;
boolean finger3FrontDone = false;
boolean finger4Done = false;
//"hand" "together"
boolean togetherBackDone1 = false;
boolean togetherBackDone2 = false;
boolean togetherFrontDone1 = false;
boolean togetherFrontDone2 = false;
int allSet1 = 0;
int allSet2 = 0;

// digitalWrite(magnValve1, (state & 0x02)); //finger0
// digitalWrite(magnValve2, (state & 0x04)); //finger1 - back
// digitalWrite(magnValve3, (state & 0x08)); //finger1 - front
// digitalWrite(magnValve4, (state & 0x10)); //finger2 - back
// digitalWrite(magnValve5, (state & 0x20)); //finger2 - front
// digitalWrite(magnValve6, (state & 0x40)); //finger3 - back
// digitalWrite(magnValve7, (state & 0x80)); //finger3 - front
// digitalWrite(magnValve8, (state & 0x100));//finger4
// digitalWrite(magnValve9, (state & 0x200));//air

void loop() {
  updateSensorData();
  if (handMode.indexOf("hand") != -1) {  //"hand" handMode
    if (airMode.indexOf("seperate") != -1) {
      //finger0
      if (finger0Done == false) {
        airPumpControl(true, 150);
        magnValveControl(0x02);
        Serial.println(sensorData[3][0]);
        //Serial.println(analogRead(backCurvature));
        while (sensorData[3][0] < finger0CurvatureValue) {  //analogRead(backCurvature)
          selectBoard(3 + 4);
          sensorData[3][0] = ads[3].readADC_SingleEnded(0);
          Serial.println(sensorData[3][0]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(backCurvature));
        finger0Done = true;
      }
      //finger1
      // if (finger0Done == true && finger1BackDone == false) {
      //   airPumpControl(true, 150);
      //   magnValveControl(0x4);
      //   Serial.println(sensorData[0][0]);
      //   //Serial.println(analogRead(frontCurvature));
      //   while (sensorData[0][0] < finger1BackCurvatureValue) {  //analogRead(frontCurvature)
      //     selectBoard(0 + 4);
      //     sensorData[0][0] = ads[0].readADC_SingleEnded(0);
      //     Serial.println(sensorData[0][0]);
      //   }
      //   airPumpControl(false, 110);
      //   magnValveControl(0x0);
      //   //Serial.println(analogRead(frontCurvature));
      //   finger1BackDone = true;
      // }
      finger1BackDone = true;
      if (finger1BackDone == true && finger1FrontDone == false) {
        airPumpControl(true, 150);
        magnValveControl(0x8);
                  selectBoard(0 + 4);
          sensorData[0][3] = ads[0].readADC_SingleEnded(3);
        Serial.println(sensorData[0][3]);

        //Serial.println(analogRead(frontCurvature));
        while (sensorData[0][3] < finger1FrontCurvatureValue) {
          selectBoard(0 + 4);
          sensorData[0][3] = ads[0].readADC_SingleEnded(3);
          Serial.println(sensorData[0][3]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(frontCurvature));
        finger1FrontDone = true;
      }
      //finger2
      if (finger1FrontDone == true && finger2BackDone == false) {
        airPumpControl(true, 150);
        magnValveControl(0x10);
        Serial.println(sensorData[1][0]);
        //Serial.println(analogRead(frontCurvature));
        while (sensorData[1][0] < finger2BackCurvatureValue) {
          selectBoard(1 + 4);
          sensorData[1][0] = ads[1].readADC_SingleEnded(0);
          Serial.println(sensorData[1][0]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(frontCurvature));
        finger2BackDone = true;
      }
      if (finger2BackDone == true && finger2FrontDone == false) {
        airPumpControl(true, 150);
        magnValveControl(0x20);
        Serial.println(sensorData[1][2]);
        //Serial.println(analogRead(frontCurvature));
        while (sensorData[1][2] < finger2FrontCurvatureValue) {
          selectBoard(1 + 4);
          sensorData[1][2] = ads[1].readADC_SingleEnded(2);
          Serial.println(sensorData[1][2]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(frontCurvature));
        finger2FrontDone = true;
      }
      //finger3
      if (finger2FrontDone == true && finger3BackDone == false) {
        airPumpControl(true, 150);
        magnValveControl(0x40);
        Serial.println(sensorData[2][0]);
        //Serial.println(analogRead(frontCurvature));
        while (sensorData[2][0] < finger3BackCurvatureValue) {
          selectBoard(2 + 4);
          sensorData[2][0] = ads[2].readADC_SingleEnded(0);
          Serial.println(sensorData[2][0]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(frontCurvature));
        finger3BackDone = true;
      }
      if (finger3BackDone == true && finger3FrontDone == false) {
        airPumpControl(true, 150);
        magnValveControl(0x80);
        Serial.println(sensorData[2][2]);
        //Serial.println(analogRead(frontCurvature));
        while (sensorData[2][2] < finger3FrontCurvatureValue) {
          selectBoard(2 + 4);
          sensorData[2][2] = ads[2].readADC_SingleEnded(2);
          Serial.println(sensorData[2][2]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(frontCurvature));
        finger3FrontDone = true;
      }
      //finger4
      if (finger3FrontDone == true && finger4Done == false) {
        airPumpControl(true, 150);
        magnValveControl(0x100);
                  selectBoard(3 + 4);
          sensorData[3][2] = ads[3].readADC_SingleEnded(2);
        Serial.println(sensorData[3][2]);
        //Serial.println(analogRead(frontCurvature));
        while (sensorData[3][2] > finger4CurvatureValue) {
          selectBoard(3 + 4);
          sensorData[3][2] = ads[3].readADC_SingleEnded(2);
          Serial.println(sensorData[3][2]);
        }
        airPumpControl(false, 110);
        magnValveControl(0x0);
        //Serial.println(analogRead(frontCurvature));
        finger4Done = true;
      }

      // for(int i= 0; i<4; i++){

      //   // back
      //   do{
      //     updateSensorData();
      //     airPumpControl(true, 150);


      //   } while(finger);

      //   //front
      //   while(){

      //   }
      // }

    } 
    // else if (airMode.indexOf("together") != -1) {
    //   if (allSet1 == 0) {
    //     if (togetherBackDone1 == false) {
    //       airPumpControl(true, 150);
    //       currentMagnState = 0x1fe;
    //       magnValveControl(currentMagnState);
    //       ///Serial.println(analogRead(backCurvature));
    //       //need change
    //       if (sensorData[3][0] >= backBaseCurvature) {
    //         currentMagnState = currentMagnState ^ 0x02;  //use xor to change one magn state
    //         magnValveControl(currentMagnState);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet1++;
    //       }
    //       if (sensorData[0][0] >= backBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet1++;
    //       }
    //       if (sensorData[1][0] >= backBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet1++;
    //       }
    //       if (sensorData[2][0] >= backBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet1++;
    //       }
    //       if (sensorData[3][2] >= backBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet1++;
    //       }
    //       if (allSet1 == 5) {
    //         airPumpControl(false, 110);
    //         magnValveControl(0x0);
    //         togetherBackDone1 = true;
    //       }
    //     }

    //     if (togetherBackDone1 == true && togetherBackDone2 == false) {
    //       for (int finger = 0; finger < 5; finger++) {
    //         magnValveControl(1 >> finger);  //need change
    //         while (magnBackControlCriteria[finger] < fingerBackCurvature[finger]) {
    //           airPumpControl(true, 155);
    //           //关闭当前阀门
    //         }
    //         airPumpControl(false, 0);  // need change
    //       }
    //       togetherBackDone2 = true;
    //     }
    //     //   Serial.println(analogRead(biggerCur));
    //     //   if (analogRead(biggerCur) >= biggerCurvature) {
    //     //     airPumpControl(false, 110);
    //     //     magnValveControl(0x0);
    //     //     Serial.println(analogRead(biggerCur));
    //     //     togetherDone2 = true;
    //     //   }
    //     // }
    //   }
    //   if (allSet2 == 0) {
    //     if (togetherFrontDone1 == false) {
    //       airPumpControl(true, 150);
    //       magnValveControl(0x1fe >> 8);  //need change
    //       ///Serial.println(analogRead(backCurvature));
    //       //need change
    //       if (sensorData[0][2] >= frontBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet2++;
    //       }
    //       if (sensorData[1][2] >= frontBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet2++;
    //       }
    //       if (sensorData[2][2] >= frontBaseCurvature) {
    //         magnValveControl(0x2);
    //         //Serial.println(analogRead(backCurvature));
    //         allSet2++;
    //       }
    //       if (allSet2 == 3) {
    //         airPumpControl(false, 110);
    //         magnValveControl(0x0);
    //         togetherFrontDone1 = true;
    //       }
    //     }

    //     if (togetherFrontDone1 == true && togetherFrontDone2 == false) {
    //       for (int finger1 = 0; finger1 < 3; finger1++) {
    //         magnValveControl(1 >> finger1);  //need change
    //         while (magnFrontControlCriteria[finger1] < fingerFrontCurvature[finger1]) {
    //           airPumpControl(true, 155);
    //           //关闭当前阀门
    //         }
    //       }
    //       togetherFrontDone2 = true;
    //     }
    //   }

    //   // Serial.println(analogRead(curvature0));
    // }
    //放气
    if (Serial.available()) {
      String command = Serial.readStringUntil('/n');
      if (command.indexOf("release") != -1) {
        airPumpControl(false, 110);
        magnValveControl(0x1fe);
        delay(500);
      }
    }
  }
}

void updateSensorData() {
  for (int board = 0; board < 4; board++) {
    selectBoard(board + 4);
    for (int channel = 0; channel < 4; channel++) {
      sensorData[board][channel] = ads[board].readADC_SingleEnded(channel);
      if (board == 0 && channel == 3) {
        int old02 = sensorData[0][2];
        sensorData[0][2] = sensorData[0][3];
        sensorData[0][3] = old02;
      }
    }
  }
}

void selectBoard(byte board) {
  Wire.beginTransmission(0x70);  //I2C地址 拓展板地址
  Wire.write(1 << board);
  Wire.endTransmission();
}

void airPumpControl(boolean state, int speed) {
  if (state == true) {
    digitalWrite(airPump1_1, HIGH);
    digitalWrite(airPump1_2, LOW);
    analogWrite(airPumpEna1, speed);
  } else {
    digitalWrite(airPump1_1, LOW);
    digitalWrite(airPump1_2, LOW);
  }
}

void magnValveControl(int state) {
  //digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  digitalWrite(magnValve2, (state & 0x04));
  digitalWrite(magnValve3, (state & 0x08));
  digitalWrite(magnValve4, (state & 0x10));
  digitalWrite(magnValve5, (state & 0x20));
  digitalWrite(magnValve6, (state & 0x40));
  digitalWrite(magnValve7, (state & 0x80));
  digitalWrite(magnValve8, ((state & 0x100) >> 8));
  digitalWrite(magnValve9, ((state & 0x200) >> 8));
}

int findBaseCurvature(int a, int b, int c, int d, int e) {
  int min = a;
  if (b < min) {
    min = b;
  }
  if (c < min) {
    min = c;
  }
  if (d < min) {
    d = min;
  }
  if (e < min) {
    e = min;
  }
  return min;
}

int findBaseCurvature2(int a, int b, int c) {
  int min = a;
  if (b < min) {
    min = b;
  }
  if (c < min) {
    min = c;
  }
  return min;
}