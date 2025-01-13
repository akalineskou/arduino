#include <Preferences.h>

#include "ACControl.h"
#include "ACMode.h"
#include "Directive.h"
#include "InfraredTransmitter.h"
#include "Secrets.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"
#include "TimeDelay.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

Preferences rebootPreferences;
TimeDelay rebootTimeDelay(REBOOT_DELAY);

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(23, acMode);
TemperatureSensor temperatureSensor(32);
TemperatureData temperatureData(temperatureSensor, acMode);
ACControl acControl(infraredTransmitter, temperatureData);
WebServerHelper webServerHelper(acControl, temperatureSensor, infraredTransmitter, temperatureData, acMode);

void setup() {
#if DEBUG
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);
#endif

  enableLoopWDT();

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

#if DEBUG
    Serial.println();
    Serial.println("Restoring data after reboot...");
#endif

    // restore data after reboot
    const bool enabled = rebootPreferences.getBool("ac-e");
    rebootPreferences.remove("ac-e");

    if (enabled) {
      acControl.enable();
    }
#if DEBUG
    Serial.printf("acControl.enabled %d.\n", acControl.isEnabled());
#endif

    infraredTransmitter.lastACCommand = sToACCommand(rebootPreferences.getString("it-lacc").c_str());
    rebootPreferences.remove("it-lacc");
#if DEBUG
    Serial.printf("infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    infraredTransmitter.lightToggled = rebootPreferences.getBool("it-lt");
    rebootPreferences.remove("it-lt");
#if DEBUG
    Serial.printf("infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    temperatureData.temperatureTarget = rebootPreferences.getInt("td-tt");
    rebootPreferences.remove("td-tt");
#if DEBUG
    Serial.printf("temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif

#if DEBUG
    Serial.println();
#endif
  }

  temperatureSensor.setup();
  infraredTransmitter.setup();
  WifiHelper::setup(wifiSSID, wifiPassword);
  webServerHelper.setup(webServerAuthUsername, webServerAuthPassword);

  if (!isReboot) {
#if DEBUG
    Serial.println("Turning off A/C on start.");
#endif

    // start with sending Οff in case of an unexpected reboot (force command since it starts Οff)
    infraredTransmitter.sendCommand(Off, true, true);
  }
}

void loop() {
  if (rebootTimeDelay.delayPassed()) {
#if DEBUG
    Serial.println("Storing data for reboot...");
#endif

    // save data to restore after reboot
    rebootPreferences.putBool("is-reboot", true);

    rebootPreferences.putBool("ac-e", acControl.isEnabled());
#if DEBUG
    Serial.printf("acControl.enabled %d.\n", acControl.isEnabled());
#endif

    rebootPreferences.putString("it-lacc", ACCommands[infraredTransmitter.lastACCommand]);
#if DEBUG
    Serial.printf("infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    rebootPreferences.putBool("it-lt", infraredTransmitter.lightToggled);
#if DEBUG
    Serial.printf("infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    rebootPreferences.putInt("td-tt", temperatureData.temperatureTarget);
#if DEBUG
    Serial.printf("temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif

#if DEBUG
    Serial.println("Rebooting...");
    Serial.println();
#endif
    // wait for watchdog timer
    delay(3600 * 1000);
  }

  temperatureSensor.loop();
  acControl.loop();
  webServerHelper.loop();
}
