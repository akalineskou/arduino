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

constexpr unsigned long REBOOT_DELAY_MS = 12 * 3600 * 1000;
TimeDelay rebootTimeDelay(REBOOT_DELAY_MS);
Preferences rebootPreferences;

#define PIN_IR_TRANSMITTER 23
#define PIN_TEMPERATURE_SENSOR_IN_1 32
#define PIN_TEMPERATURE_SENSOR_OUT_1 22

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(PIN_IR_TRANSMITTER, acMode);
TemperatureSensorManager temperatureSensorManager(
  new TemperatureSensor*[1]{new TemperatureSensor(PIN_TEMPERATURE_SENSOR_IN_1)},
  1,
  new TemperatureSensor*[1]{new TemperatureSensor(PIN_TEMPERATURE_SENSOR_OUT_1)},
  1);
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

  // restore data after reboot
  if (rebootPreferences.isKey("is-reboot")) {
    rebootPreferences.remove("is-reboot");

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
  } else {
    // start with sending off in case of an unexpected reboot (force command
    // since it starts off)
    infraredTransmitter.sendCommand(Off, true, true);
  }

  temperatureSensorManager.setup();
  infraredTransmitter.setup();
  WifiHelper::setup(wifiSSID, wifiPassword);
  webServerHelper.setup(webServerAuthUsername, webServerAuthPassword);
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
