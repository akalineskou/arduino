#pragma once

#include <ir_Haier.h>

#include "ACCommand.h"
#include "ACMode.h"

class InfraredTransmitter {
  int pin;
  DatabaseHelper &databaseHelper;
  ACMode &acMode;

  IRHaierAC160 irSend;

 public:
  ACCommand lastACCommand;
  bool lightToggled;

  InfraredTransmitter(int pin, DatabaseHelper &databaseHelper, ACMode &acMode);

  void setup();

  void sendCommand(ACCommand acCommand);
};
