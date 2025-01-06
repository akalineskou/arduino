#pragma once

#include <IRsend.h>
#include "millisDelay.h"
#include "ACCommand.h"
#include "IRData.h"

class InfraredTransmitter {
  int pin;
  IRData irData;

  IRsend irSend;
  millisDelay timeDelay;

public:
  ACCommand lastACCommand;

  InfraredTransmitter(
    int pin,
    const IRData &irData
  );

  void setup();

  void sendCommand(ACCommand acCommand, bool forceSend = false);
};
