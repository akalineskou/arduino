#pragma once

#include "ACCommand.h"
#include "ACMode.h"
#include "Directive.h"

class InfraredTransmitter {
  int pin;
  DatabaseHelper &databaseHelper;
  ACMode &acMode;

  IR_SEND_CLASS irSend;

 public:
  ACCommand lastACCommand;
  bool lightToggled;

  InfraredTransmitter(int pin, DatabaseHelper &databaseHelper, ACMode &acMode);

  void setup();

  void sendCommand(ACCommand acCommand);
};
