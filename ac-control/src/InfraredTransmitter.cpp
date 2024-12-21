#include <Arduino.h>
#include "InfraredTransmitter.h"

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

  Serial.printf("Sending A/C command: %s.\n", ACCommands[acCommand]);

  if (Off == acCommand) {
    irSend.sendHaierAC160(acData.Off);
  } else if (Start == acCommand) {
    irSend.sendHaierAC160(acData.Start);
  } else if (Stop == acCommand) {
    irSend.sendHaierAC160(acData.Stop);
  }
}
