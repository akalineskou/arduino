#include <Preferences.h>
#include <esp_task_wdt.h>

#include "ACControl.h"
#include "ACMode.h"
#include "DatabaseHelper.h"
#include "Directive.h"
#include "InfraredTransmitter.h"
#include "SdHelper.h"
#include "Secrets.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"
#include "TimeDelay.h"
#include "TimeHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

Preferences errorPreferences;
TimeDelay rebootTimeDelay(APP_REBOOT_DELAY);
TimeDelay temperatureSensorDatabaseTimeDelay(APP_TEMPERATURE_SENSOR_DATABASE_DELAY, true);
#if APP_DEBUG_HEAP
TimeDelay debugHeapTimeDelay(APP_DEBUG_HEAP_DELAY, true);
#endif

TimeHelper timeHelper;
SdHelper sdHelper(18, 19, 23, 5);
DatabaseHelper databaseHelper(timeHelper);

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(22, databaseHelper, acMode);
TemperatureSensor temperatureSensor(32);
TemperatureData temperatureData(temperatureSensor, acMode);
ACControl acControl(infraredTransmitter, temperatureData, databaseHelper);
WebServerHelper webServerHelper(
  acControl, temperatureSensor, infraredTransmitter, temperatureData, databaseHelper, acMode
);

void increaseErrorPreferencesCount();

void setup() {
  // for some reason needed for the IR transmitter to work
  Serial.begin(115200);

#if APP_DEBUG
  // wait for serial monitor to start completely.
  delay(2500);
#endif

  enableLoopWDT();

  // increase watchdog timer
  esp_task_wdt_init(10, true);

  temperatureSensor.setup();
  infraredTransmitter.setup();

  // https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html
  errorPreferences.begin("error", false);

  if (errorPreferences.isKey("count") && errorPreferences.getInt("count") > 5) {
#if APP_DEBUG
    Serial.println("Turning off A/C on error count more than 5.");
#endif

    acControl.off();

    errorPreferences.remove("count");
  }

  WifiHelper::setup(wifiSSID, wifiPassword);
  webServerHelper.setup(webServerAuthUsername, webServerAuthPassword);
  timeHelper.setup();
  if (!sdHelper.setup()) {
    increaseErrorPreferencesCount();

    while (true) {}
  }
  if (!databaseHelper.setup()) {
    increaseErrorPreferencesCount();

    while (true) {}
  }

  // remove on successful initialization
  errorPreferences.remove("count");

  const auto preference = databaseHelper.selectPreference();
  if (preference != nullptr) {
#if APP_DEBUG
    Serial.println();
    Serial.println("Restoring data...");
#endif

    acMode = sToACMode(preference->acMode.c_str());
#if APP_DEBUG
    Serial.printf("acMode %s.\n", ACModes[acMode]);
#endif

    acControl.enabled = preference->acEnabled;
#if APP_DEBUG
    Serial.printf("acControl.enabled %d.\n", acControl.enabled);
#endif

    acControl.temperatureStart = preference->acTemperatureStart;
#if APP_DEBUG
    Serial.printf("acControl.acTemperatureStart %d.\n", acControl.temperatureStart);
#endif

    acControl.temperatureStop = preference->acTemperatureStop;
#if APP_DEBUG
    Serial.printf("acControl.temperatureStop %d.\n", acControl.temperatureStop);
#endif

    infraredTransmitter.lastACCommand = sToACCommand(preference->irLastACCommand.c_str());
#if APP_DEBUG
    Serial.printf("infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    infraredTransmitter.lightToggled = preference->irLightToggled;
#if APP_DEBUG
    Serial.printf("infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    temperatureData.temperatureTarget = preference->tdTemperatureTarget;
#if APP_DEBUG
    Serial.printf("temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif

#if APP_DEBUG
    Serial.println();
#endif

    delete preference;
  }
}

void loop() {
  if (rebootTimeDelay.delayPassed()) {
#if APP_DEBUG
    Serial.println("Rebooting...");
    Serial.println();
#endif
    // wait for watchdog timer
    delay(3600 * 1000);
  }

  temperatureSensor.loop();
  acControl.loop();
  webServerHelper.loop();
  timeHelper.loop();

  if (temperatureSensorDatabaseTimeDelay.delayPassed()) {
    databaseHelper.insertTemperatureReading(
      temperatureData.getTemperature(),
      temperatureData.temperatureTargetStart(),
      temperatureData.temperatureTargetStop(),
      temperatureData.getHumidity()
    );
  }

#if APP_DEBUG && APP_DEBUG_HEAP
  if (debugHeapTimeDelay.delayPassed()) {
    Serial.printf(
      "\nHeap size: %d, Free Heap: %d, Min Free Heap: %d, Max Alloc Heap: %d\n",
      ESP.getHeapSize(),
      heap_caps_get_free_size(MALLOC_CAP_8BIT),
      heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT),
      heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)
    );
  }
#endif
}

void increaseErrorPreferencesCount() {
  if (!errorPreferences.isKey("count")) {
    errorPreferences.putInt("count", 0);
  }

  errorPreferences.putInt("count", errorPreferences.getInt("count") + 1);
}
