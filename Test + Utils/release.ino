#define magnValve0 52
#define magnValve1 53
#define magnValve5 49

#define airPump1 22
#define airPump2 23
#define airPumpEna 13

void magnValveControl(int state) {
  digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  // digitalWrite(magnValve3, (state & 0x04));
  // digitalWrite(magnValve4, (state & 0x08));
  digitalWrite(magnValve5, (state & 0x20));
  // digitalWrite(magnValve6, (state & 0x20));
}

void setup() {
  // put your setup code here, to run once:
  pinMode(airPump1, OUTPUT);
  pinMode(airPump2, OUTPUT);
  pinMode(airPumpEna, OUTPUT);
  //电磁阀
  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve5, OUTPUT);

  magnValveControl(0x23);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
}
