#include <Preferences.h>

#include "ACControl.h"
#include "ACMode.h"
#include "Directive.h"
#include "InfraredTransmitter.h"
#include "Secrets.h"
#include "TemperatureData.h"
#include "TemperatureSensorManager.h"
#include "TimeDelay.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

Preferences rebootPreferences;
TimeDelay rebootTimeDelay(12 * 3600 * 1000);

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(23, acMode);
TemperatureSensorManager temperatureSensorManager(new TemperatureSensor*[1]{new TemperatureSensor(32)}, 1);
TemperatureData temperatureData(temperatureSensorManager, acMode);
ACControl acControl(infraredTransmitter, temperatureData);
WebServerHelper webServerHelper(acControl, temperatureSensorManager, infraredTransmitter, temperatureData, acMode);

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

  // restore data after reboot
  if (rebootPreferences.isKey("is-reboot")) {
    rebootPreferences.remove("is-reboot");

    isReboot = true;

#if DEBUG
    Serial.println();
#endif

    // restore data after reboot
    acControl.enabled = rebootPreferences.getBool("ac-e");
    rebootPreferences.remove("ac-e");
#if DEBUG
    Serial.printf("Restoring acControl.enabled %d.\n", acControl.enabled);
#endif

    infraredTransmitter.lastACCommand = sToACCommand(rebootPreferences.getString("ir-lacc").c_str());
    rebootPreferences.remove("ir-lacc");
#if DEBUG
    Serial.printf("Restoring infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    infraredTransmitter.lightToggled = rebootPreferences.getBool("ir-lt");
    rebootPreferences.remove("ir-lt");
#if DEBUG
    Serial.printf("Restoring infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    temperatureData.temperatureTarget = rebootPreferences.getInt("td-tt");
    rebootPreferences.remove("td-tt");
#if DEBUG
    Serial.printf("Restoring temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif
  }

  temperatureSensorManager.setup();
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
  if (rebootTimeDelay.finished()) {
#if DEBUG
    Serial.println("Rebooting...");
#endif

    // save data to restore after reboot
    rebootPreferences.putBool("is-reboot", true);

    rebootPreferences.putBool("ac-e", acControl.enabled);
#if DEBUG
    Serial.printf("Storing acControl.enabled %d.\n", acControl.enabled);
#endif

    rebootPreferences.putString("ir-lacc", ACCommands[infraredTransmitter.lastACCommand]);
#if DEBUG
    Serial.printf("Storing infraredTransmitter.lastACCommand %s.\n", ACCommands[infraredTransmitter.lastACCommand]);
#endif

    rebootPreferences.putBool("ir-lt", infraredTransmitter.lightToggled);
#if DEBUG
    Serial.printf("Storing infraredTransmitter.lightToggled %d.\n", infraredTransmitter.lightToggled);
#endif

    rebootPreferences.putInt("td-tt", temperatureData.temperatureTarget);
#if DEBUG
    Serial.printf("Storing temperatureData.temperatureTarget %d.\n", temperatureData.temperatureTarget);
#endif

#if DEBUG
    Serial.println("Waiting for watch dog timer...");
    Serial.println();
#endif
    delay(3600 * 1000);
  }

  temperatureSensorManager.loop();
  acControl.loop();
  webServerHelper.loop();
}
