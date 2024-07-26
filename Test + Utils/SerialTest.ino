#include <math.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
//电磁阀
#define magnValve0 22  //负压泵
#define magnValve1 23
#define magnValve2 24
#define magnValve3 25
#define magnValve4 26
#define magnValve5 27
#define magnValve6 28
#define magnValve7 29
#define magnValve8 30
#define magnValve9 31  // 连接大气
//air pump
#define airPump1 36
#define airPump2 37
#define airPumpEna 13
//

Adafruit_ADS1X15 ads[4];
//IIC channel selection
void selectBoard(byte board);
int sensorData[4][4];  //data by board 前三排中间三根手指食指中指无名指 第四排前两项大拇指后两项小拇指
//airPump
void airPumpControl(boolean state, int speed);
//magnValve
void magnValveControl(int state);

void setup() {
  //airPump
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //magnValve
  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve2, OUTPUT);
  pinMode(magnValve3, OUTPUT);
  pinMode(magnValve4, OUTPUT);
  pinMode(magnValve5, OUTPUT);
  pinMode(magnValve6, OUTPUT);
  pinMode(magnValve7, OUTPUT);
  pinMode(magnValve8, OUTPUT);
  pinMode(magnValve9, OUTPUT);
  //curvatureSensor
  // pinMode(backCurvature, INPUT);
  // pinMode(frontCurvature, INPUT);

  Serial.begin(115200);
  Wire.begin();
  for (int board = 0; board < 4; board++) {
    selectBoard(board);
    ads[board].begin();
    ads[board].setGain(GAIN_ONE);  //?
  }
}

void loop() {
  //put your main code here, to run repeatedly:
  //IIC板data receiving
  updateSensorData();
  Serial.print("thumb curvature: ");
  Serial.println(sensorData[3][2]);

}

void updateSensorData() {
  //  for (int board = 0; board < 4; board++) {
  int board = 3;
  selectBoard(board + 4);
  for (int channel = 0; channel < 4; channel++) {
    sensorData[board][channel] = ads[board].readADC_SingleEnded(channel);
    if (board == 0 && channel == 3) {
      int old02 = sensorData[0][2];
      sensorData[0][2] = sensorData[0][3];
      sensorData[0][3] = old02;
    }
  }
  // }
}

void selectBoard(byte board) {
  Wire.beginTransmission(0x70);  //I2C地址 拓展板地址
  Wire.write(1 << board);
  Wire.endTransmission();
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
  digitalWrite(magnValve2, (state & 0x04));
  digitalWrite(magnValve3, (state & 0x08));
  digitalWrite(magnValve4, (state & 0x10));
  digitalWrite(magnValve5, (state & 0x20));
  digitalWrite(magnValve6, (state & 0x40));
  digitalWrite(magnValve7, (state & 0x80));
  digitalWrite(magnValve8, (state & 0x100) >> 8);
  digitalWrite(magnValve9, (state & 0x200) >> 8);
}
