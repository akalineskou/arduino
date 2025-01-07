#include "ACControl.h"
#include "Directive.h"

ACControl::ACControl(
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData
): infraredTransmitter(infraredTransmitter),
   temperatureData(temperatureData) {
  enabled = false;
}

void ACControl::loop() {
  control();
}

void ACControl::enable() {
  enabled = true;

#if DEBUG
  Serial.println("A/C control enabled.");
#endif
}

void ACControl::disable() {
  enabled = false;

  infraredTransmitter.sendCommand(Off, true);

#if DEBUG
  Serial.println("A/C control disabled.");
#endif
}

void ACControl::control() {
  if (!enabled) {
    return;
  }

  if (temperatureData.temperatureSensorsInFailed()) {
#if DEBUG
    Serial.println("Temperature sensors in failed.");
#endif

    disable();
    return;
  }

  if (infraredTransmitter.lastACCommand != Start && temperatureData.temperatureStartReached()) {
    infraredTransmitter.sendCommand(Start);
  } else if (infraredTransmitter.lastACCommand != Stop && temperatureData.temperatureStopReached()) {
    infraredTransmitter.sendCommand(Stop);
  }
}
