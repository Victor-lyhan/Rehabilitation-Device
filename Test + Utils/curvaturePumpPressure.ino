//手指弯曲度和气泵气压的关系
//单根手指
#define magnValve0 52
#define magnValve1 53

#define airPump1 22
#define airPump2 23
#define airPumpEna 13

#define airPressure0 A5
#define airPressure1 A4

#define curvature0 A11
#define curvature1 A9

// void airPumpControl(boolean state, int speed);
// void magnValveControl(int state);

//f
//Pressure[5] = { 350, 290, 410, 372, 452 };

void setup() {
  // put your setup code here, to run once:
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //电磁阀
  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  //气压传感器
  pinMode(airPressure0, INPUT);
  pinMode(airPressure1, INPUT);

  Serial.begin(9600);
}

int minCurvature = 260;
void loop() {
  // put your main code here, to run repeatedly:
  while (analogRead(curvature1) <= 300) {

    while (analogRead(curvature1) < minCurvature) {
      airPumpControl(true, 170);
      magnValveControl(0x2);
    }
    magnValveControl(0);
    Serial.print("frontCurvature: ");
    Serial.print(analogRead(curvature1));
    Serial.print(", airPressure: ");
    Serial.println(analogRead(airPressure1));
    minCurvature += 5;
    delay(2000);
    
  }
  magnValveControl(0);
  airPumpControl(false,100);
  if (Serial.available()) {
    String command = Serial.readStringUntil('/n');
    if (command.indexOf("release") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x2);
    }
  }
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
  // digitalWrite(magnValve5, (state & 0x10));
  // digitalWrite(magnValve6, (state & 0x20));
}
