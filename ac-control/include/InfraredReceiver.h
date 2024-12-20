#pragma once

#include <IRrecv.h>

class InfraredReceiver {
  int pin;

  IRrecv irRecv;

public:
  explicit InfraredReceiver(
    const int pin
  ): pin(pin),
     irRecv(pin, 1024, 50, true) {
  }

  void setup();

  void loop();
};
