#include "Arduino.h"

void setup() {
  // fast baud for IR receiver
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  Serial.println("setup!");
}

void loop() {
}
