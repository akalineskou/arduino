#include "ACControl.h"
#include "Directive.h"

ACControl::ACControl(InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData):
    infraredTransmitter(infraredTransmitter),
    temperatureData(temperatureData),
    turnOffTimeDelay(TimeDelay(AC_CONTROL_TURN_OFF_DELAY)) {
  enabled = false;
}

void ACControl::loop() {
  control();
}

void ACControl::enable() {
  enabled = true;

  // restart time delay on enable
  turnOffTimeDelay.restart();

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

bool ACControl::isEnabled() const {
  return enabled;
}

void ACControl::start() {
  infraredTransmitter.sendCommand(Start);

  turnOffTimeDelay.restart();
}

void ACControl::stop() {
  infraredTransmitter.sendCommand(Stop);

  turnOffTimeDelay.restart();
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

  if (turnOffTimeDelay.delayPassed()) {
#if DEBUG
    Serial.println("Turn off time delay passed.");
#endif

    disable();
    return;
  }

  if (infraredTransmitter.lastACCommand != Start && temperatureData.temperatureStartReached()) {
    start();
  } else if (infraredTransmitter.lastACCommand != Stop && temperatureData.temperatureStopReached()) {
    stop();
  }
}
