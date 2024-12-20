#include <Arduino.h>
#include "InfraredTransmitter.h"

void InfraredTransmitter::setup() {
  irSend.begin();
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand, const bool forceSend) {
  if (!forceSend || millis() - lastCall < 500) {
    // ir transmit every 0.5s
    return;
  }
  lastCall = millis();

  if (acCommand == lastCommand) {
    // same AC command, ignoring send
    return;
  }
  lastCommand = acCommand;

  switch (acCommand) {
    case Off:
      Serial.println("Sending AC command: Off");
      irSend.sendHaierAC160(acData.off);
      break;

    case Start:
      Serial.println("Sending AC command: Start");
      irSend.sendHaierAC160(acData.start);
      break;

    case Stop:
      Serial.println("Sending AC command: Stop");
      irSend.sendHaierAC160(acData.stop);
      break;

    default:
      break;
  }
}
