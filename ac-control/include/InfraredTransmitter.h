#pragma once

#include <IRsend.h>
#include "ACCommand.h"
#include "IRData.h"

class InfraredTransmitter {
  int pin;
  IRData irData;

  IRsend irSend;
  unsigned long lastCall;

public:
  ACCommand lastACCommand;

  InfraredTransmitter(
    int pin,
    const IRData &irData
  );

  void setup();

  void sendCommand(ACCommand acCommand, bool forceSend = false);
};
