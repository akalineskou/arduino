#include "ACControl.h"
#include "Directive.h"

ACControl::ACControl(
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData
): infraredTransmitter(infraredTransmitter),
   temperatureData(temperatureData) {
  enabled = false;
  wasChanged = false;
}

void ACControl::setup() const {
  // start with sending off in case of an unexpected reboot (force command since it starts off)
  infraredTransmitter.sendCommand(Off, true, true);
}

void ACControl::loop() {
  control();
}

bool ACControl::isEnabled() const {
  return enabled;
}

void ACControl::toggleStatus(const bool triggerChange) {
  enabled = !enabled;
  wasChanged = triggerChange;

#if DEBUG
  if (triggerChange) {
    Serial.printf("A/C control %s.\n", enabled ? "enabled" : "disabled");
  }
#endif
}

void ACControl::control() {
  if (!enabled) {
    if (wasChanged) {
      // turn off on button disable change (since it starts disabled, this means it was enabled then disabled)
      infraredTransmitter.sendCommand(Off, true);
    }

    wasChanged = false;

    return;
  }

  if (infraredTransmitter.lastACCommand != Start && temperatureData.temperatureStartReached()) {
    infraredTransmitter.sendCommand(Start);
  } else if (infraredTransmitter.lastACCommand != Stop && temperatureData.temperatureStopReached()) {
    infraredTransmitter.sendCommand(Stop);
  } else if (wasChanged) {
    // temperature is between thresholds, start A/C with Stop command when button was enabled
    infraredTransmitter.sendCommand(Stop, true);
  }

  wasChanged = false;

  if (temperatureData.temperatureSensorsInFailed()) {
    // disable button enabled to stop the A/C if the temperature sensor failed
    toggleStatus();
  }
}
