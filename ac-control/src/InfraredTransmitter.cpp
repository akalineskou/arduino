#include <Arduino.h>
#include "InfraredTransmitter.h"
#include "Serial.h"

InfraredTransmitter::InfraredTransmitter(
  const int pin,
  const IRData &irData
): pin(pin),
   irData(irData),
   irSend(pin),
   timeDelay(millisDelay()) {
  lastACCommand = Off;
}

void InfraredTransmitter::setup() {
  irSend.begin();

  // allow IR transmit every 0.5s
  timeDelay.start(0.5 * 1000);
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand, const bool forceSend) {
  if (!timeDelay.justFinished() && !forceSend) {
    return;
  }
  timeDelay.repeat();

  if (acCommand == lastACCommand) {
    // same A/C command, ignoring send
    return;
  }
  lastACCommand = acCommand;

  D_printf("Sending A/C command: %s.\n", ACCommands[acCommand]);

  if (Off == acCommand) {
    irSend.sendHaierAC160(irData.Off);
  } else if (Start == acCommand) {
    irSend.sendHaierAC160(irData.Start);
  } else if (Stop == acCommand) {
    irSend.sendHaierAC160(irData.Stop);
  }
}
