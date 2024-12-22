#include "ACControl.h"

ACControl::ACControl(
  ButtonEnabled &buttonEnabled,
  InfraredTransmitter &infraredTransmitter,
  const TemperatureData &temperatureData
): buttonEnabled(buttonEnabled),
   infraredTransmitter(infraredTransmitter),
   temperatureData(temperatureData) {
}

void ACControl::loop() const {
  control();
}

void ACControl::control() const {
  if (!buttonEnabled.enabled) {
    if (buttonEnabled.hasChanged) {
      // turn off on button disable change (since it starts disabled, this means it was enabled then disabled)
      infraredTransmitter.sendCommand(Off, true);
    }

    return;
  }

  if (infraredTransmitter.lastACCommand != Start && temperatureData.temperatureStart()) {
    infraredTransmitter.sendCommand(Start);
  } else if (infraredTransmitter.lastACCommand != Stop && temperatureData.temperatureStop()) {
    infraredTransmitter.sendCommand(Stop);
  } else if (buttonEnabled.hasChanged) {
    // temperature is between thresholds, start A/C with Stop command when button was enabled
    infraredTransmitter.sendCommand(Stop, true);
  }
}
