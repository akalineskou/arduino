#include "ACControl.h"
#include "Directive.h"

ACControl::ACControl(InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData):
    infraredTransmitter(infraredTransmitter),
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

  if (temperatureData.temperatureSensorFailed()) {
#if DEBUG
    Serial.println("Temperature sensor failed.");
#endif

    disable();
    return;
  }

  if (infraredTransmitter.lastACCommand != Start && temperatureData.temperatureStartReached()) {
    infraredTransmitter.sendCommand(Start);
  } else if (infraredTransmitter.lastACCommand != Stop && temperatureData.temperatureStopReached()) {
    infraredTransmitter.sendCommand(Stop);
  } else if (infraredTransmitter.lastACCommand == Off) {
    // was off and temperature is between thresholds, turn on A/C with stop as to not run
    infraredTransmitter.sendCommand(Stop);
  }
}
