#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

MPU6050 mpu6050(Wire);

//IIC
Adafruit_ADS1X15 ads[4];
void selectBoard(byte board);
int fingerSensorData[4][4];
int mpu6050Data[3];
void updateSensorData();


void setup() {
  Serial.begin(9600);
  Wire.begin();
  selectBoard(3);
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  for (int i = 0; i < 4; i++) {
    selectBoard(i + 4);
    ads[i].begin();
    ads[i].setGain(GAIN_ONE);
  }
}

void loop() {
  mpu6050.update();
  updateSensorData();
  Serial.print("angleX : ");
  Serial.print(mpu6050Data[0]);
  Serial.print("\tangleY : ");
  Serial.print(mpu6050Data[1]);
  Serial.print("\tangleZ : ");
  Serial.println(mpu6050Data[2]);
}

void updateSensorData() {
  //finger sensor data
  for (int board = 0; board < 4; board++) {
    selectBoard(board + 4);
    for (int channel = 0; channel < 4; channel++) {
      fingerSensorData[board][channel] = ads[board].readADC_SingleEnded(channel);
      // if (board == 0 && channel == 3) {
      //   int old02 = sensorData[0][2];
      //   sensorData[0][2] = sensorData[0][3];
      //   sensorData[0][3] = old02;
      // }
    }
  }
  //mpu6050
  selectBoard(3);
  mpu6050Data[0] = mpu6050.getAngleX();
  mpu6050Data[1] = mpu6050.getAngleY();
  mpu6050Data[2] = mpu6050.getAngleZ();
}

void selectBoard(byte board) {
  Wire.beginTransmission(0x70);  //I2C地址 拓展板地址
  Wire.write(1 << board);
  Wire.endTransmission();
}
