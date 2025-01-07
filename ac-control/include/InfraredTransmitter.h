#pragma once

#include <ir_Haier.h>
#include "ACCommand.h"
#include "ACMode.h"
#include "TimeDelay.h"

class InfraredTransmitter {
  int pin;
  ACMode acMode;

  IRHaierAC160 irSend;
  TimeDelay timeDelay;

public:
  ACCommand lastACCommand;
  bool lightToggled;

  InfraredTransmitter(
    int pin,
    const ACMode &acMode
  );

  void setup();

  void sendCommand(ACCommand acCommand, bool forceTimeDelay = false, bool forceCommand = false);
};
