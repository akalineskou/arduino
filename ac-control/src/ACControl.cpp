#include "ACControl.h"

void ACControl::loop() const {
  if (buttonEnabled.hasChanged) {
    infraredTransmitter.sendCommand(buttonEnabled.enabled ? Start : Off, true);
  }

  control();
}

void ACControl::control() const {
  if (!buttonEnabled.enabled) {
    return;
  }

  if (infraredTransmitter.lastCommand == Start) {
    if (temperatureSensor.temperature >= temperatureTarget + temperatureThresholdUp) {
      infraredTransmitter.sendCommand(Stop);
    }
  } else if (infraredTransmitter.lastCommand == Stop) {
    if (temperatureSensor.temperature <= temperatureTarget - temperatureThresholdDown) {
      infraredTransmitter.sendCommand(Start);
    }
  } else {
    infraredTransmitter.sendCommand(Start);
  }
}
