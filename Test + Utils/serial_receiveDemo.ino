// esp32端的代码
int Parameter[3] = { 0, 0, 0 };

void Decode(String packet) {
  String Temp = "";
  if (packet.length() > 0) {
    int j = 0;
    for (int i = 0; i < packet.length(); i++) {
      if (packet[i] == ' ') {
        Parameter[j] = Temp.toInt();
        Temp = "";
        j++;
      } else if (packet[i] == '/') {
        Parameter[j] = Temp.toInt();
        Temp = "";
        j = 0;
      } else {
        Temp += packet[i];
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() {
  String inString = "";
  while (Serial2.available() > 0) {
    inString += char(Serial2.read());
    delay(10);  // 延时函数用于等待字符完全进入缓冲区，可以尝试没有延时，输出结果会是什么
  }
  Decode(inString);

  
}

