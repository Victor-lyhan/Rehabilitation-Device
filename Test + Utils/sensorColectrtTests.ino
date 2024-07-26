//sensor
//board a
#define a0 5  //sensors
#define a1 6
#define a2 7
#define z1 A2  // pwm
// #define en1 //connecting gnd - always working
//board b
#define b0 2
#define b1 3
#define b2 4
#define z2 A1  // pwm
// #define en2

int sensorSelection(char board, int channel) {
  if (board == 'a') {
    digitalWrite(a0, (channel & 0x01));
    digitalWrite(a1, (channel & 0x02));
    digitalWrite(a2, (channel & 0x04));
    return analogRead(A2);
  } else if (board == 'b') {
    digitalWrite(b0, (channel & 0x01));
    digitalWrite(b1, (channel & 0x02));
    digitalWrite(b2, (channel & 0x04));
    return analogRead(A1);
  }
}

int readData(int pin0, int pin1, int pin2, int number, int z) {
  switch (number) {
    case 0:
      digitalWrite(pin0, 0);
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 0);
      break;
    case 1:
      digitalWrite(pin0, 0);
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 1);
      break;
    case 2:
      digitalWrite(pin0, 0);
      digitalWrite(pin1, 1);
      digitalWrite(pin2, 0);
      break;
    case 3:
      digitalWrite(pin0, 0);
      digitalWrite(pin1, 1);
      digitalWrite(pin2, 1);
      break;
    case 4:
      digitalWrite(pin0, 1);
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 0);
      break;
    case 5:
      digitalWrite(pin0, 1);
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 1);
      break;
    case 6:
      digitalWrite(pin0, 1);
      digitalWrite(pin1, 1);
      digitalWrite(pin2, 0);
      break;
    case 7:
      digitalWrite(pin0, 1);
      digitalWrite(pin1, 1);
      digitalWrite(pin2, 1);
      break;
    default:
      Serial.println("nothing");
      break;
  }
  int val = analogRead(z);
  return val;
}


int sensorData[4][4];

// { sensorSelection('a', 3), sensorSelection('a', 2), sensorSelection('a', 1), sensorSelection('a', 0) },    //finger1
//                          { sensorSelection('a', 7), sensorSelection('a', 6), sensorSelection('a', 5), sensorSelection('a', 4) },    //finger2
//                          { sensorSelection('b', 3), sensorSelection('b', 2), sensorSelection('b', 1), sensorSelection('b', 0) },    //finger3
//                          { sensorSelection('b', 7), sensorSelection('b', 6), sensorSelection('b', 5), sensorSelection('b', 4) }  //thumb finger0 curv - pressure / tail finger4 curv - pressure

void setup() {
  // put your setup code here, to run once:
  pinMode(a0, OUTPUT);
  pinMode(a1, OUTPUT);
  pinMode(a2, OUTPUT);
  pinMode(b0, OUTPUT);
  pinMode(b1, OUTPUT);
  pinMode(b2, OUTPUT);
  Serial.begin(115200);
  Serial.println("working");
}

void loop() {
  // put your main code here, to run repeatedly:
  // sensorData = { { sensorSelection('a', 3), sensorSelection('a', 2), sensorSelection('a', 1), sensorSelection('a', 0) },
  //                { sensorSelection('a', 7), sensorSelection('a', 6), sensorSelection('a', 5), sensorSelection('a', 4) },
  //                { sensorSelection('b', 3), sensorSelection('b', 2), sensorSelection('b', 1), sensorSelection('b', 0) },
  //                { sensorSelection('b', 7), sensorSelection('b', 6), sensorSelection('b', 5), sensorSelection('b', 4) } };

  // for (int i = 0; i < 5; i++) {
  //   if (i == 0) {
  //     System.out.println("Finger0:");
  //     System.out.println("curvature-");
  //     System.out.println(sensorData[3[0]);
  //     System.out.println("pressure-");
  //     System.out.println(sensorData[3[1]);
  //   } else if (i == 4) {
  //     System.out.println("Finger4:");
  //     System.out.println("curvature-");
  //     System.out.println(sensorData[3][2]);
  //     System.out.println("pressure-");
  //     System.out.println(sensorData[3][3]);
  //   } else {
  //     System.out.println("Finger");
  //     System.out.println(i);
  //     System.out.println("Back ");
  //     System.out.println(" curvature-");
  //     System.out.println(sensorData[i - 1][0]);
  //     System.out.println(" pressure-");
  //     System.out.println(sensorData[i - 1][1]);
  //     System.out.println("Front ");
  //     System.out.println(" curvature-");
  //     System.out.println(sensorData[i - 1][2]);
  //     System.out.println(" pressure-");
  //     System.out.println(sensorData[i - 1][3]);
  //   }
  // }

  // digitalWrite(a0, 0);
  // digitalWrite(a1, 0);
  // digitalWrite(a2, 0);
  // Serial.println(analogRead(A2));

  Serial.print('A');
  Serial.print(' ');

  for (int i = 0; i < 8; i++) {
    int val1 = readData(5, 6, 7, i, A2);
    Serial.print(val1);
    Serial.print(' ');
    delay(5);
  }
  Serial.print(' ');
  Serial.print('B');
  Serial.print(' ');

  for (int j = 0; j < 8; j++) {
    Serial.print(readData(2, 3, 4, j, A1));
    Serial.print(' ');
    delay(5);
  }
  Serial.println(' ');
  delay(20);
}
