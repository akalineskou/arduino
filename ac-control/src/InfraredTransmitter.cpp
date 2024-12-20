#include <Arduino.h>
#include "InfraredTransmitter.h"

void InfraredTransmitter::setup() {
  irSend.begin();
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand, const bool forceSend) {
  if (Null == acCommand) {
    return;
  }

  if (!forceSend && millis() - lastCall < 500) {
    // ir transmit every 0.5s
    return;
  }
  lastCall = millis();

  if (acCommand == lastCommand) {
    // same AC command, ignoring send
    return;
  }
  lastCommand = acCommand;

  Serial.printf("Sending AC command: %s\n", ACCommands[acCommand]);

  if (Off == acCommand) {
    irSend.sendHaierAC160(acData.off);
  } else if (Start == acCommand) {
    irSend.sendHaierAC160(acData.start);
  } else if (Stop == acCommand) {
    irSend.sendHaierAC160(acData.stop);
  }
}
