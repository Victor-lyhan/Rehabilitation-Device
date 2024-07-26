#include <IRremote.h>
int RECV_PIN = 8;

IRrecv irrecv(RECV_PIN);

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
}
void loop() {
  uint8_t command = 0;
  if (irrecv.decode()) {
    Serial.print("0x");
    command = irrecv.decodedIRData.command;
    Serial.println(command, HEX);
    irrecv.resume();  // Receive the next value
  }
}
