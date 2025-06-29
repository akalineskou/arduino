#include "ACControl.h"
#include "DatabaseHelper.h"
#include "Directive.h"
#include "Preference.h"

ACControl::ACControl(
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData,
  DatabaseHelper &databaseHelper,
  ACMode &acMode
):
    infraredTransmitter(infraredTransmitter),
    temperatureData(temperatureData),
    databaseHelper(databaseHelper),
    acMode(acMode),
    turnOffTimeDelay(TimeDelay(APP_AC_CONTROL_TURN_OFF_DELAY)),
    temperatureChangeTimeDelay(TimeDelay(APP_AC_CONTROL_CHECK_DELAY)) {
  enabled = false;
  temperatureStart = -1;
  temperatureStop = -1;
  spamOff = false;
}

void ACControl::loop() {
  control();
}

void ACControl::enable() {
  enabled = true;

  // restart time delay on enable
  turnOffTimeDelay.restart();

  databaseHelper.updatePreferenceByType(AcEnabled, reinterpret_cast<void*>(enabled));

#if APP_DEBUG
  Serial.println("A/C control enabled.");
#endif
}

void ACControl::disable() {
  enabled = false;

  off();

  databaseHelper.updatePreferenceByType(AcEnabled, reinterpret_cast<void*>(enabled));

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
  databaseHelper.updatePreferenceByType(AcTemperatureStart, reinterpret_cast<void*>(temperatureStart));
  databaseHelper.updatePreferenceByType(AcTemperatureStop, reinterpret_cast<void*>(temperatureStop));
}

void ACControl::stop() {
  infraredTransmitter.sendCommand(Stop);

  turnOffTimeDelay.restart();
  temperatureChangeTimeDelay.restart();

  temperatureStart = -1;
  temperatureStop = temperatureData.getTemperature();

  databaseInsert(Stop);
  databaseHelper.updatePreferenceByType(AcTemperatureStart, reinterpret_cast<void*>(temperatureStart));
  databaseHelper.updatePreferenceByType(AcTemperatureStop, reinterpret_cast<void*>(temperatureStop));
}

void ACControl::changeMode() {
  acMode = ACModesOther[acMode];

  temperatureStart = -1;
  temperatureStop = -1;

  temperatureData.updateTemperatures(true);

  databaseHelper.updatePreferenceByType(AcMode, ACModes[acMode]);
  databaseHelper.updatePreferenceByType(AcTemperatureStart, reinterpret_cast<void*>(temperatureStart));
  databaseHelper.updatePreferenceByType(AcTemperatureStop, reinterpret_cast<void*>(temperatureStop));
  databaseHelper.updatePreferenceByType(
    TdTemperatureTarget,
    reinterpret_cast<void*>(temperatureData.temperatureTarget)
  );
}

ACCommand ACControl::lastACCommand() const {
  return infraredTransmitter.lastACCommand;
}

void ACControl::control() {
  if (spamOff) {
    if (enabled) {
      disable();
    } else {
      infraredTransmitter.sendCommand(Off);
    }
  }

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

    off();
    return;
  }

  if (temperatureChangeTimeDelay.delayPassed()) {
    if (infraredTransmitter.lastACCommand == Start && temperatureStart != -1) {
      if (temperatureData.temperatureStartReached(temperatureStart, false)) {
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

        databaseHelper.updatePreferenceByType(AcTemperatureStart, reinterpret_cast<void*>(temperatureStart));
      }
    } else if (infraredTransmitter.lastACCommand == Stop && temperatureStop != -1) {
      if (temperatureData.temperatureStopReached(temperatureStop, false)) {
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

        databaseHelper.updatePreferenceByType(AcTemperatureStop, reinterpret_cast<void*>(temperatureStop));
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
