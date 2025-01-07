#include <Preferences.h>
#include "millisDelay.h"
#include "ACControl.h"
#include "ACMode.h"
#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "Secrets.h"
#include "Serial.h"
#include "TemperatureData.h"
#include "TemperatureSensorManager.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

// https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
constexpr unsigned long REBOOT_DELAY_MS = 1 * 3600 * 1000;
millisDelay rebootDelay;
// https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html
Preferences rebootPreferences;

#define PIN_BUTTON_ENABLED 4
#define PIN_IR_TRANSMITTER 23
#define PIN_TEMPERATURE_SENSOR_IN_1 32
#define PIN_TEMPERATURE_SENSOR_OUT_1 22

ACMode acMode(Heat);
ButtonEnabled buttonEnabled(PIN_BUTTON_ENABLED);
InfraredTransmitter infraredTransmitter(PIN_IR_TRANSMITTER, acMode);
TemperatureSensorManager temperatureSensorManager(
  new TemperatureSensor *[1]{new TemperatureSensor(PIN_TEMPERATURE_SENSOR_IN_1)}, 1,
  new TemperatureSensor *[1]{new TemperatureSensor(PIN_TEMPERATURE_SENSOR_OUT_1)}, 1
);
TemperatureData temperatureData(temperatureSensorManager, acMode);
ACControl acControl(buttonEnabled, infraredTransmitter, temperatureData);
WebServerHelper webServerHelper(buttonEnabled, temperatureSensorManager, infraredTransmitter, temperatureData, acMode);

void setup() {
  D_SerialBegin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  rebootDelay.start(REBOOT_DELAY_MS);
  enableLoopWDT();

  rebootPreferences.begin("reboot", false);

  buttonEnabled.setup();
  temperatureSensorManager.setup();
  infraredTransmitter.setup();
  WifiHelper::setup(wifiSSID, wifiPassword);
  webServerHelper.setup(webServerAuthUsername, webServerAuthPassword);

  // restore A/C state after reboot
  if (!rebootPreferences.isKey("ac-state")) {
    return;
  }

  // get A/C state
  const String acState = rebootPreferences.getString("ac-state");
  rebootPreferences.remove("ac-state");

  // get and restore temperature target (even when off)
  temperatureData.temperatureTarget = rebootPreferences.getInt("temp-target");
  rebootPreferences.remove("temp-target");

  infraredTransmitter.lightToggled = rebootPreferences.getBool("light-toggled");
  rebootPreferences.remove("light-toggled");

  D_printf("Restoring A/C state: %s, temperature target: %d\n", ACCommands[infraredTransmitter.lastACCommand], temperatureData.temperatureTarget);

  if (acState == ACCommands[Off]) {
    return;
  }

  buttonEnabled.enabled = true;

  if (acState == ACCommands[Start]) {
    infraredTransmitter.lastACCommand = Start;
  } else {
    infraredTransmitter.lastACCommand = Stop;
  }
}

void loop() {
  if (rebootDelay.justFinished()) {
    D_println("Rebooting...");

    // save A/C state to restore after reboot
    rebootPreferences.putString("ac-state", ACCommands[infraredTransmitter.lastACCommand]);
    rebootPreferences.putInt("temp-target", temperatureData.temperatureTarget);
    rebootPreferences.putBool("light-toggled", infraredTransmitter.lightToggled);

    D_printf("Saving A/C state: %s, temperature target: %d\n", ACCommands[infraredTransmitter.lastACCommand], temperatureData.temperatureTarget);

    D_println("Waiting for watch dog timer...");
    delay(3600 * 1000);
  }

  buttonEnabled.loop();
  temperatureSensorManager.loop();
  acControl.loop();
  webServerHelper.loop();
}
