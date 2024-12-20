#pragma once

#include <IRsend.h>
#include "ACCommand.h"
#include "ACData.h"

class InfraredTransmitter {
  int pin;
  ACData acData;

  IRsend irSend;
  unsigned long lastCall = 0;

public:
  ACCommand lastCommand = Null;

  InfraredTransmitter(
    const int pin,
    const ACData &acData
  ): pin(pin),
     acData(acData),
     irSend(pin) {
  }

  void setup();

  void sendCommand(ACCommand acCommand, bool forceSend = false);
};
