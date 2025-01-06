#include "InfraredReceiver.h"

#define PIN_IR_RECEIVER 2

InfraredReceiver infraredReceiver(PIN_IR_RECEIVER);

void setup() {
  // fast baud for IR receiver
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  infraredReceiver.setup();
}

void loop() {
  infraredReceiver.loop();
}