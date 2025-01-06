#include <Preferences.h>
#include "millisDelay.h"
#include "ACControl.h"
#include "ACMode.h"
#include "ButtonEnabled.h"
// #include "InfraredReceiver.h"
#include "InfraredTransmitter.h"
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

ACMode acMode(Heat);
ButtonEnabled buttonEnabled(4);
InfraredTransmitter infraredTransmitter(16, IRData(acMode));
TemperatureSensorManager temperatureSensorManager(new TemperatureSensor *[1]{new TemperatureSensor(0)}, 1, {}, 0);
TemperatureData temperatureData(temperatureSensorManager, acMode);
ACControl acControl(buttonEnabled, infraredTransmitter, temperatureData);
// InfraredReceiver infraredReceiver(2);

WebServerHelper webServerHelper(buttonEnabled, temperatureSensorManager, infraredTransmitter, temperatureData, acMode);

void setup() {
  // fast baud for IR receiver
  D_SerialBegin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  rebootDelay.start(REBOOT_DELAY_MS);
  enableLoopWDT();

  rebootPreferences.begin("reboot", false);

  buttonEnabled.setup();
  temperatureSensorManager.setup();
  infraredTransmitter.setup();
  // infraredReceiver.setup();

  WifiHelper::setup();
  webServerHelper.setup();

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

    D_printf("Saving A/C state: %s, temperature target: %d\n", ACCommands[infraredTransmitter.lastACCommand], temperatureData.temperatureTarget);

    D_println("Waiting for watch dog timer...");
    delay(3600 * 1000);
  }

  buttonEnabled.loop();
  temperatureSensorManager.loop();
  acControl.loop();
  // infraredReceiver.loop();

  webServerHelper.loop();
}
