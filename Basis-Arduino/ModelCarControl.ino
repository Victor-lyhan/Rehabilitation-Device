//IRremote
#include <IRremote.h>
//SR04
#include "SR04.h"
//PID
extern "C" {
#include "pid.h"
}
//MPU6050
#include <MPU6050_tockn.h>
#include <Wire.h>

//left right motors
#define left1 11
#define left2 10
#define leftEnable 9
#define right1 5
#define right2 4
#define rightEnable 6
//SR04
#define TRIG_PIN 13
#define ECHO_PIN 12

//SR04
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long a;

//IRremote
int RECV_PIN = 8;
IRrecv irrecv(RECV_PIN);

//pid
float anglePid[3] = { 1, 0, 0 };
pid_type_def angleLoop;

//MPU6050
MPU6050 mpu6050(Wire);


//motor control
void motorControl(int speedLeft, int speedRight) {
  if (speedLeft < 0) {
    analogWrite(leftEnable, abs(speedLeft));
    digitalWrite(left1, HIGH);  // left return
    digitalWrite(left2, LOW);

  } else if (speedLeft > 0) {
    analogWrite(leftEnable, abs(speedLeft));
    digitalWrite(left1, LOW);  // left return
    digitalWrite(left2, HIGH);
  } else {
    analogWrite(leftEnable, 0);
    digitalWrite(left1, LOW);  // left stop
    digitalWrite(left2, LOW);
  }
  if (speedRight < 0) {
    analogWrite(rightEnable, abs(speedRight));
    digitalWrite(right1, LOW);  // right return
    digitalWrite(right2, HIGH);

  } else if (speedRight > 0) {
    analogWrite(rightEnable, abs(speedRight));
    digitalWrite(right1, HIGH);  // right return
    digitalWrite(right2, LOW);
  } else {
    analogWrite(rightEnable, 0);
    digitalWrite(right1, LOW);  // right stop
    digitalWrite(right2, LOW);
  }
}

void setup() {
  //IR remote
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  //motors
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(leftEnable, OUTPUT);
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(rightEnable, OUTPUT);
  //pid initialization
  PID_init(&angleLoop, PID_POSITION, anglePid, 90, 10);
  //MPU6050
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}
uint8_t command = 0;
int mpu6050Position = 0;
float startPosition = 0;
int setSpeed = 0;

void loop() {

  if (irrecv.decode()) {
    command = irrecv.decodedIRData.command;
    irrecv.resume();  // Receive the next value
  }
  switch (command) {
    case 0xC:  // forward - 1
      //motorControl(200, 200);
      setSpeed = 200;
      break;
    case 0x5E:  //bakcward - 3
      //motorControl(-200, -200);
      setSpeed = -200;
      break;
    case 0x42:  //leftturn - 7
      motorControl(-110, 110);
      break;
    case 0x4A:  //rightturn - 9
      motorControl(110, -110);
      break;
    case 0x45:  //stop - CH-
      motorControl(0, 0);
      setSpeed = 0;
      break;
    case 0x1C:  // auto-avoid - 5
      motorControl(150, 150);
      a = sr04.Distance();
      if (a <= 30) {
        motorControl(0, 0);
        delay(500);
        motorControl(-100, 100);
        delay(500);
      }
      break;
    case 0x43:  //MPU6050 control
      mpu6050.update();
      if (mpu6050Position == 0) {
        startPosition = mpu6050.getAngleZ();
        mpu6050Position = 1;
      }
      float currentPosition = mpu6050.getAngleZ();
      PID_calc(&angleLoop, currentPosition, startPosition);
      motorControl(setSpeed - angleLoop.out, setSpeed + angleLoop.out);
      break;
  }
}



/*//motor tests
#define left1 11
#define left2 10
#define leftEnable 9
#define right1 5
#define right2 4
#define rightEnable 3

void setup() {
  // put your setup code here, to run once:
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(leftEnable, OUTPUT);
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(rightEnable, OUTPUT);
}

void motorControl(int speedLeft, int speedRight) {
  if (speedLeft < 0) {
    analogWrite(leftEnable, abs(speedLeft));
    digitalWrite(left1, HIGH);  // left return
    digitalWrite(left2, LOW);

  } else if (speedLeft > 0) {
    analogWrite(leftEnable, abs(speedLeft));
    digitalWrite(left1, LOW);  // left return
    digitalWrite(left2, HIGH);
  } else {
    analogWrite(leftEnable, 0);
    digitalWrite(left1, LOW);  // left stop
    digitalWrite(left2, LOW);
  }
   if (speedRight < 0) {
    analogWrite(leftEnable, abs(speedRight));
    digitalWrite(left1, HIGH);  // right return
    digitalWrite(left2, LOW);

  } else if (speedRight > 0) {
    analogWrite(leftEnable, abs(speedRight));
    digitalWrite(left1, LOW);  // right return
    digitalWrite(left2, HIGH);
  } else {
    analogWrite(leftEnable, 0);
    digitalWrite(left1, LOW);  // right stop
    digitalWrite(left2, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(leftEnable, 150);
  digitalWrite(left1, HIGH);  // left return
  digitalWrite(left2, LOW);
  analogWrite(rightEnable, 150);  // right return
  digitalWrite(right1, LOW);
  digitalWrite(right2, HIGH);
}
*/
