#include <Preferences.h>
#include <esp_task_wdt.h>

#include "ACControl.h"
#include "ACMode.h"
#include "DatabaseHelper.h"
#include "Directive.h"
#include "InfraredTransmitter.h"
#include "SdHelper.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"
#include "TimeDelay.h"
#include "TimeHelper.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"
#if APP_IR_RECEIVER
  #include "InfraredReceiver.h"
#endif

Preferences errorPreferences;
TimeDelay rebootTimeDelay(APP_REBOOT_DELAY);
TimeDelay temperatureSensorDatabaseTimeDelay(APP_TEMPERATURE_SENSOR_DATABASE_DELAY, true);
#if APP_DEBUG_HEAP
TimeDelay debugHeapTimeDelay(APP_DEBUG_HEAP_DELAY, true);
#endif

TimeHelper timeHelper;
SdHelper sdHelper(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_SS);
DatabaseHelper databaseHelper(timeHelper);

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(PIN_IR_TRANSMITTER, databaseHelper, acMode);
TemperatureSensor temperatureSensor(PIN_TEMP_SENSOR);
TemperatureData temperatureData(temperatureSensor, databaseHelper, acMode);
ACControl acControl(infraredTransmitter, temperatureData, databaseHelper, acMode);
WebServerHelper webServerHelper(acControl, timeHelper);

#if APP_IR_RECEIVER
InfraredReceiver infraredReceiver(PIN_IR_RECEIVER, acControl, temperatureData);
#endif

void increaseErrorPreferencesCount();

void setup() {
  // for some reason needed for the IR transmitter to work
  Serial.begin(115200);

#if defined(PIN_ONBOARD_LED)
  digitalWrite(PIN_ONBOARD_LED, LOW);
  neopixelWrite(PIN_ONBOARD_LED, 0, 0, 0);
#endif

#if APP_DEBUG
  // wait for serial monitor to start completely.
  delay(2500);
#endif

  enableLoopWDT();

  // increase watchdog timer
  esp_task_wdt_init(10, true);

  temperatureSensor.setup();
  infraredTransmitter.setup();
#if APP_IR_RECEIVER
  infraredReceiver.setup();
#endif

  // https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html
  errorPreferences.begin("error", false);
  if (!errorPreferences.isKey("count")) {
    errorPreferences.putInt("count", 0);
  }

  if (errorPreferences.getInt("count") > 5) {
#if APP_DEBUG
    Serial.println("Turning off A/C on error count more than 5.");
#endif

    acControl.off();

    errorPreferences.remove("count");
  }

  WifiHelper::setup();
  webServerHelper.setup();
  timeHelper.setup();
  if (!sdHelper.setup()) {
    increaseErrorPreferencesCount();

    // wait for watchdog timer
    delay(3600 * 1000);
  }
  if (!databaseHelper.setup()) {
    increaseErrorPreferencesCount();

    // wait for watchdog timer
    delay(3600 * 1000);
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

    acControl.turnOffInsteadOfStop = preference->acTurnOffInsteadOfStop;
#if APP_DEBUG
    Serial.printf("acControl.turnOffInsteadOfStop %d.\n", acControl.turnOffInsteadOfStop);
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

#if APP_IR_RECEIVER
  infraredReceiver.loop();
#endif
  temperatureSensor.loop();
  temperatureData.loop();
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
  errorPreferences.putInt("count", errorPreferences.getInt("count") + 1);
}
