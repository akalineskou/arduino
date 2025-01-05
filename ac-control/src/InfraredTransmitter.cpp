#include <Arduino.h>
#include "InfraredTransmitter.h"
#include "Serial.h"

InfraredTransmitter::InfraredTransmitter(
  const int pin,
  const IRData &irData
): pin(pin),
   irData(irData),
   irSend(pin) {
  lastCall = 0;

  lastACCommand = Off;
}

void InfraredTransmitter::setup() {
  irSend.begin();
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand, const bool forceSend) {
  if (millis() - lastCall < 500 && !forceSend) {
    // ir transmit every 0.5s
    return;
  }
  lastCall = millis();

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
