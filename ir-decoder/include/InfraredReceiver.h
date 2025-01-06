#pragma once

#include <IRrecv.h>

class InfraredReceiver {
  int pin;
  IRrecv irRecv;

public:
  explicit InfraredReceiver(int pin);

  void setup();

  void loop();
};
