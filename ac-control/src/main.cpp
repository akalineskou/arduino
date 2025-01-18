#include <Preferences.h>

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
#include "esp_task_wdt.h"

Preferences rebootPreferences;
TimeDelay rebootTimeDelay(APP_REBOOT_DELAY);
TimeDelay temperatureSensorDatabaseTimeDelay(APP_TEMPERATURE_SENSOR_DATABASE_DELAY, true);

TimeHelper timeHelper;
SdHelper sdHelper(18, 19, 23, 5);
DatabaseHelper databaseHelper(timeHelper);

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(22, acMode);
TemperatureSensor temperatureSensor(32);
TemperatureData temperatureData(temperatureSensor, acMode);
ACControl acControl(infraredTransmitter, temperatureData, databaseHelper);
WebServerHelper webServerHelper(
  acControl, temperatureSensor, infraredTransmitter, temperatureData, databaseHelper, acMode
);

void setup() {
  // for some reason needed for the IR transmitter to work
  Serial.begin(115200);

#if APP_DEBUG
  // wait for serial monitor to start completely.
  delay(2500);
#endif

  enableLoopWDT();

  // increase watchdog timer to 10 seconds
  esp_task_wdt_init(10, true);

  // https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html
  rebootPreferences.begin("reboot", false);

  bool isReboot = false;

  // // simulate reboot
  // rebootPreferences.putBool("is-reboot", true);
  // rebootPreferences.putBool("ac-e", true);
  // rebootPreferences.putString("it-lacc", "Start");
  // rebootPreferences.putBool("it-lt", true);
  // rebootPreferences.putInt("td-tt", 205);

  // restore data after reboot
  if (rebootPreferences.isKey("is-reboot")) {
    rebootPreferences.remove("is-reboot");

    isReboot = true;

#if APP_DEBUG
    Serial.println();
    Serial.println("Restoring data after reboot...");
#endif

    // restore data after reboot
    const bool enabled = rebootPreferences.getBool("ac-e");
    rebootPreferences.remove("ac-e");

    if (enabled) {
      acControl.enable();
    }
#if APP_DEBUG
    Serial.printf("acControl.enabled %d.\n", acControl.isEnabled());
#endif

    infraredTransmitter.lastACCommand = sToACCommand(rebootPreferences.getString("it-lacc").c_str());
    rebootPreferences.remove("it-lacc");
#if APP_DEBUG
    Serial.printf("infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    infraredTransmitter.lightToggled = rebootPreferences.getBool("it-lt");
    rebootPreferences.remove("it-lt");
#if APP_DEBUG
    Serial.printf("infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    temperatureData.temperatureTarget = rebootPreferences.getInt("td-tt");
    rebootPreferences.remove("td-tt");
#if APP_DEBUG
    Serial.printf("temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif

#if APP_DEBUG
    Serial.println();
#endif
  }

  temperatureSensor.setup();
  infraredTransmitter.setup();
  WifiHelper::setup(wifiSSID, wifiPassword);
  webServerHelper.setup(webServerAuthUsername, webServerAuthPassword);
  timeHelper.setup();
  sdHelper.setup();
  databaseHelper.setup();

  if (!isReboot) {
#if APP_DEBUG
    Serial.println("Turning off A/C on start.");
#endif

    // start with sending Οff in case of an unexpected reboot (force command since it starts Οff)
    acControl.off();
  }
}

void loop() {
  if (rebootTimeDelay.delayPassed()) {
#if APP_DEBUG
    Serial.println("Storing data for reboot...");
#endif

    // save data to restore after reboot
    rebootPreferences.putBool("is-reboot", true);

    rebootPreferences.putBool("ac-e", acControl.isEnabled());
#if APP_DEBUG
    Serial.printf("acControl.enabled %d.\n", acControl.isEnabled());
#endif

    rebootPreferences.putString("it-lacc", ACCommands[infraredTransmitter.lastACCommand]);
#if APP_DEBUG
    Serial.printf("infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    rebootPreferences.putBool("it-lt", infraredTransmitter.lightToggled);
#if APP_DEBUG
    Serial.printf("infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    rebootPreferences.putInt("td-tt", temperatureData.temperatureTarget);
#if APP_DEBUG
    Serial.printf("temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif

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
    databaseHelper.insertTemperatureReading(temperatureSensor.getTemperature(), temperatureSensor.getHumidity());

#if APP_DEBUG
    Serial.printf(
      "\nHeap size: %d, Free Heap: %d, Min Free Heap: %d, Max Alloc Heap: %d\n\n",
      ESP.getHeapSize(),
      heap_caps_get_free_size(MALLOC_CAP_8BIT),
      heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT),
      heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)
    );
#endif
  }
}
