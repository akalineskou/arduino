#pragma once

#include <ir_Haier.h>
#include "millisDelay.h"
#include "ACCommand.h"
#include "ACMode.h"

class InfraredTransmitter {
  int pin;
  ACMode acMode;

  IRHaierAC160 irSend;
  millisDelay timeDelay;

public:
  ACCommand lastACCommand;
  bool lightToggled;

  InfraredTransmitter(
    int pin,
    const ACMode &acMode
  );

  void setup();

  void sendCommand(ACCommand acCommand, bool forceSend = false);
};
