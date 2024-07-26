//任务：电磁阀依次打开 让每个气道压力不同 一个电磁阀打开 判断是否达到预设气压 是 关闭当前电磁阀并打开下一个电磁阀

#define magnValve1 52
#define magnValve2 53
#define magnValve3 50
#define magnValve4 51
#define magnValve5 48
#define magnValve6 49
#define airPump1 22
#define airPump2 23
#define airPumpEna 13
#define airPressure1 A5
#define airPressure2 A4
#define airPressure3 A3
#define airPressure4 A2
#define airPressure5 A1

void airPumpControl(boolean state, int speed);
void magnValveControl(int state);

//电磁阀对应的不同气压数上线
int magnValvePressure[5] = { 350, 290, 410, 372, 452 };

void setup() {
  // put your setup code here, to run once:
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //电磁阀
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve2, OUTPUT);
  pinMode(magnValve3, OUTPUT);
  pinMode(magnValve4, OUTPUT);
  pinMode(magnValve5, OUTPUT);
  pinMode(magnValve6, OUTPUT);
  //气压传感器
  pinMode(airPressure1, INPUT);
  pinMode(airPressure2, INPUT);
  pinMode(airPressure3, INPUT);
  pinMode(airPressure4, INPUT);
  pinMode(airPressure5, INPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String command = Serial.readStringUntil('/n');
    if (command.indexOf("start") != -1) {
      for (int i = 0; i < 5; i++) {
        magnValveControl(1 << i);
        while (analogRead(A5 - i) < magnValvePressure[i]) {
          airPumpControl(true, 140);
          Serial.print(analogRead(airPressure1));
          Serial.print(", ");
          Serial.print(analogRead(airPressure2));
          Serial.print(", ");
          Serial.print(analogRead(airPressure3));
          Serial.print(", ");
          Serial.print(analogRead(airPressure4));
          Serial.print(", ");
          Serial.println(analogRead(airPressure5));
        }
        // Serial.println(analogRead(A5 - i));
      }
      magnValveControl(0);
        
        airPumpControl(false, 150);
    } 
    if (command.indexOf("release") != -1) {
      magnValveControl(0x3f);
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
  digitalWrite(magnValve1, (state & 0x01));
  digitalWrite(magnValve2, (state & 0x02));
  digitalWrite(magnValve3, (state & 0x04));
  digitalWrite(magnValve4, (state & 0x08));
  digitalWrite(magnValve5, (state & 0x10));
  digitalWrite(magnValve6, (state & 0x20));
}
