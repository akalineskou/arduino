#include <Preferences.h>
#include "millisDelay.h"
#include "ACControl.h"
#include "ACMode.h"
#include "Directive.h"
#include "InfraredTransmitter.h"
#include "Secrets.h"
#include "TemperatureData.h"
#include "TemperatureSensorManager.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

// https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
constexpr unsigned long REBOOT_DELAY_MS = 12 * 3600 * 1000;
millisDelay rebootDelay;
// https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html
Preferences rebootPreferences;

#define PIN_IR_TRANSMITTER 23
#define PIN_TEMPERATURE_SENSOR_IN_1 32
#define PIN_TEMPERATURE_SENSOR_OUT_1 22

ACMode acMode(Heat);
InfraredTransmitter infraredTransmitter(PIN_IR_TRANSMITTER, acMode);
TemperatureSensorManager temperatureSensorManager(
  new TemperatureSensor *[1]{new TemperatureSensor(PIN_TEMPERATURE_SENSOR_IN_1)}, 1,
  new TemperatureSensor *[1]{new TemperatureSensor(PIN_TEMPERATURE_SENSOR_OUT_1)}, 1
);
TemperatureData temperatureData(temperatureSensorManager, acMode);
ACControl acControl(infraredTransmitter, temperatureData);
WebServerHelper webServerHelper(acControl, temperatureSensorManager, infraredTransmitter, temperatureData, acMode);

void setup() {
#if DEBUG
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);
#endif

  rebootDelay.start(REBOOT_DELAY_MS);
  enableLoopWDT();

  rebootPreferences.begin("reboot", false);

  temperatureSensorManager.setup();
  infraredTransmitter.setup();
  acControl.setup();
  WifiHelper::setup(wifiSSID, wifiPassword);
  webServerHelper.setup(webServerAuthUsername, webServerAuthPassword);

  // restore A/C state after reboot
  if (!rebootPreferences.isKey("ac-state")) {
    return;
  }

  // get A/C state
  const String acState = rebootPreferences.getString("ac-state");
  rebootPreferences.remove("ac-state");

  // get and restore temperatureTarget (even when off)
  temperatureData.temperatureTarget = rebootPreferences.getInt("temp-target");
  rebootPreferences.remove("temp-target");

  // get and restore lightToggled (even when off)
  infraredTransmitter.lightToggled = rebootPreferences.getBool("light-toggled");
  rebootPreferences.remove("light-toggled");

#if DEBUG
  Serial.println();
  Serial.printf("Restoring A/C state: %s, temperature target: %d, light toggled: %d\n",
                ACCommands[infraredTransmitter.lastACCommand], temperatureData.temperatureTarget, infraredTransmitter.lightToggled);
#endif

  if (acState == ACCommands[Off]) {
    return;
  }

  acControl.toggleStatus(false);

  if (acState == ACCommands[Start]) {
    infraredTransmitter.lastACCommand = Start;
  } else {
    infraredTransmitter.lastACCommand = Stop;
  }
}

void loop() {
  if (rebootDelay.justFinished()) {
#if DEBUG
    Serial.println();
    Serial.println("Rebooting...");
#endif

    // save A/C state to restore after reboot
    rebootPreferences.putString("ac-state", ACCommands[infraredTransmitter.lastACCommand]);
    rebootPreferences.putInt("temp-target", temperatureData.temperatureTarget);
    rebootPreferences.putBool("light-toggled", infraredTransmitter.lightToggled);

#if DEBUG
    Serial.printf("Saving A/C state: %s, temperature target: %d, light toggled: %d\n",
                  ACCommands[infraredTransmitter.lastACCommand], temperatureData.temperatureTarget, infraredTransmitter.lightToggled);
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
