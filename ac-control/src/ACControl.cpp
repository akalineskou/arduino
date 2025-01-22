#include "ACControl.h"
#include "DatabaseHelper.h"
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

  databaseHelper.updatePreferenceAcEnabled(enabled);

#if APP_DEBUG
  Serial.println("A/C control enabled.");
#endif
}

void ACControl::disable() {
  enabled = false;

  infraredTransmitter.sendCommand(Off);

  databaseHelper.updatePreferenceAcEnabled(enabled);

#if APP_DEBUG
  Serial.println("A/C control disabled.");
#endif
}

void ACControl::off() const {
  infraredTransmitter.sendCommand(Off);

  databaseInsert(Off);
}

void ACControl::start() {
  infraredTransmitter.sendCommand(Start);

  turnOffTimeDelay.restart();

  databaseInsert(Start);
}

void ACControl::stop() {
  infraredTransmitter.sendCommand(Stop);

  turnOffTimeDelay.restart();

  databaseInsert(Stop);
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

void ACControl::databaseInsert(const ACCommand acCommand) const {
  databaseHelper
    .insertCommand(ACCommands[acCommand], temperatureData.getTemperature(), temperatureData.temperatureTarget);

  databaseHelper.insertTemperatureReading(
    temperatureData.getTemperature(),
    temperatureData.temperatureTargetStart(),
    temperatureData.temperatureTargetStop(),
    temperatureData.getHumidity()
  );
}
