#include "ACControl.h"
#include "ACMode.h"
#include "ButtonEnabled.h"
// #include "InfraredReceiver.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TemperatureSensorManager.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

ACMode acMode(Heat);
ButtonEnabled buttonEnabled(4);
InfraredTransmitter infraredTransmitter(16, IRData(acMode));
TemperatureSensor *arrSens[1] = {new TemperatureSensor(0)};
TemperatureSensorManager temperatureSensorManager(arrSens, {});
TemperatureData temperatureData(temperatureSensorManager, acMode);
ACControl acControl(buttonEnabled, infraredTransmitter, temperatureData);
// InfraredReceiver infraredReceiver(2);

WebServerHelper webServerHelper(buttonEnabled, temperatureSensorManager, infraredTransmitter, temperatureData, acMode);

void setup() {
  // fast baud for IR receiver
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  buttonEnabled.setup();
  temperatureSensorManager.setup();
  infraredTransmitter.setup();
  // infraredReceiver.setup();

  WifiHelper::setup();
  webServerHelper.setup();
}

void loop() {
  buttonEnabled.loop();
  temperatureSensorManager.loop();
  acControl.loop();
  // infraredReceiver.loop();

  webServerHelper.loop();
}
