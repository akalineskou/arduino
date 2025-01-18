#include "ACControl.h"
#include "Directive.h"

ACControl::ACControl(
  InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData, DatabaseHelper &databaseHelper
):
    infraredTransmitter(infraredTransmitter),
    temperatureData(temperatureData),
    databaseHelper(databaseHelper),
    turnOffTimeDelay(TimeDelay(APP_AC_CONTROL_TURN_OFF_DELAY)) {
  enabled = false;
}

void ACControl::loop() {
  control();
}

void ACControl::enable() {
  enabled = true;

  // restart time delay on enable
  turnOffTimeDelay.restart();

#if APP_DEBUG
  Serial.println("A/C control enabled.");
#endif
}

void ACControl::disable() {
  enabled = false;

  infraredTransmitter.sendCommand(Off, true);

#if APP_DEBUG
  Serial.println("A/C control disabled.");
#endif
}

bool ACControl::isEnabled() const {
  return enabled;
}

void ACControl::off() const {
  infraredTransmitter.sendCommand(Off, true);

  insertCommand(Off);
}

void ACControl::start() {
  infraredTransmitter.sendCommand(Start);

  turnOffTimeDelay.restart();

  insertCommand(Start);
}

void ACControl::stop() {
  infraredTransmitter.sendCommand(Stop);

  turnOffTimeDelay.restart();

  insertCommand(Stop);
}

void ACControl::control() {
  if (!enabled) {
    return;
  }

  if (temperatureData.temperatureSensorFailed()) {
#if APP_DEBUG
    Serial.println("Temperature sensor failed.");
#endif

    disable();
    return;
  }

  if (turnOffTimeDelay.delayPassed()) {
#if APP_DEBUG
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

void ACControl::insertCommand(const ACCommand acCommand) const {
  databaseHelper
    .insertCommand(ACCommands[acCommand], temperatureData.getTemperature(), temperatureData.temperatureTarget);
}
