#include "ACControl.h"
#include "DatabaseHelper.h"
#include "Directive.h"
#include "Preference.h"

ACControl::ACControl(
  InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData, DatabaseHelper &databaseHelper
):
    infraredTransmitter(infraredTransmitter),
    temperatureData(temperatureData),
    databaseHelper(databaseHelper),
    turnOffTimeDelay(TimeDelay(APP_AC_CONTROL_TURN_OFF_DELAY)),
    temperatureChangeTimeDelay(TimeDelay(APP_AC_CONTROL_CHECK_DELAY)) {
  enabled = false;
  temperatureStart = -1;
  temperatureStop = -1;
}

void ACControl::loop() {
  control();
}

void ACControl::enable() {
  enabled = true;

  // restart time delay on enable
  turnOffTimeDelay.restart();

  databaseHelper.updatePreferenceByType(AcEnabled, (void*) enabled);

#if APP_DEBUG
  Serial.println("A/C control enabled.");
#endif
}

void ACControl::disable() {
  enabled = false;

  off();

  databaseHelper.updatePreferenceByType(AcEnabled, (void*) enabled);

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
  temperatureChangeTimeDelay.restart();

  temperatureStart = temperatureData.getTemperature();
  temperatureStop = -1;

  databaseInsert(Start);
  databaseHelper.updatePreferenceByType(AcTemperatureStart, (void*) temperatureStart);
  databaseHelper.updatePreferenceByType(AcTemperatureStart, (void*) temperatureStop);
}

void ACControl::stop() {
  infraredTransmitter.sendCommand(Stop);

  turnOffTimeDelay.restart();
  temperatureChangeTimeDelay.restart();

  temperatureStart = -1;
  temperatureStop = temperatureData.getTemperature();

  databaseInsert(Stop);
  databaseHelper.updatePreferenceByType(AcTemperatureStart, (void*) temperatureStart);
  databaseHelper.updatePreferenceByType(AcTemperatureStop, (void*) temperatureStop);
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

  if (temperatureChangeTimeDelay.delayPassed()) {
    if (infraredTransmitter.lastACCommand == Start && temperatureStart != -1) {
      if (temperatureData.temperatureStartReached(temperatureStart)) {
        // still not running, set to Stop before rebooting
        databaseHelper.updatePreferenceByType(IrLastACCommand, ACCommands[Stop]);

#if APP_DEBUG
        Serial.println("Temperature change not detected, rebooting...");
#endif

        // wait for watchdog timer
        delay(3600 * 1000);
      } else {
        // temperature changed, no need to check
        temperatureStart = -1;

        databaseHelper.updatePreferenceByType(AcTemperatureStart, (void*) temperatureStart);
      }
    } else if (infraredTransmitter.lastACCommand == Stop && temperatureStop != -1) {
      if (temperatureData.temperatureStopReached(temperatureStop)) {
        // still running, set to Start before rebooting
        databaseHelper.updatePreferenceByType(IrLastACCommand, ACCommands[Start]);

#if APP_DEBUG
        Serial.println("Temperature change not detected, stopping again");
#endif

        // wait for watchdog timer
        delay(3600 * 1000);
      } else {
        // temperature changed, no need to check
        temperatureStop = -1;

        databaseHelper.updatePreferenceByType(AcTemperatureStop, (void*) temperatureStop);
      }
    }
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
    temperatureData.getHumidity(),
    true
  );
}
