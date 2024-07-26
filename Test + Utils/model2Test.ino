#define magnValve1 23  //大拇指
#define magnValve9 31
#define airPump1_1 38
#define airPump1_2 39
#define airPumpEna1 13
#define a0 15  //sensors
#define a1 16
#define a2 17
#define z1 18  // pwm
// #define en1 //connecting gnd - always working
#define b0 19
#define b1 20
#define b2 21
#define z2 22  // pwm
// #define en2


void setup() {
  // put your setup code here, to run once:
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve9, OUTPUT);
  pinMode(airPump1_1, OUTPUT);
  pinMode(airPump1_2, OUTPUT);
  pinMode(airPumpEna1, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String command = Serial.readStringUntil('/n');
    if (command.indexOf("start") != -1) {
      airPumpControl(true, 150);
      magnValveControl(0x02);
    }
    if (command.indexOf("stop") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x0);
      delay(500);
    }
    if (command.indexOf("release") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x202);
      delay(500);
    }
    if (command.indexOf("close") != -1) {
      airPumpControl(false, 110);
      magnValveControl(0x0);
      delay(500);
    }
  }
}

void magnValveControl(int state) {
  //digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  digitalWrite(magnValve9, ((state & 0x200) >> 8));
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

//模拟量扩展口
void sensorSelection(char board, int channel) {
  if (board == 'a') {
    digitalWrite(a0, (channel & 0x01));
    digitalWrite(a1, (channel & 0x02));
    digitalWrite(a2, (channel & 0x04));
  } else if (board == 'b') {
    digitalWrite(b0, (channel & 0x01));
    digitalWrite(b1, (channel & 0x02));
    digitalWrite(b2, (channel & 0x04));
  }
}
