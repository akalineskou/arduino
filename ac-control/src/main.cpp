#include "ACControl.h"
#include "ACMode.h"
#include "ButtonEnabled.h"
// #include "InfraredReceiver.h"
#include "InfraredTransmitter.h"
#include "IRData.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"
#include "WebServerHelper.h"
#include "WifiHelper.h"

ACMode acMode(Heat);
ButtonEnabled buttonEnabled(4);
TemperatureSensor temperatureSensor(0);
InfraredTransmitter infraredTransmitter(16, IRData(acMode));
TemperatureData temperatureData(temperatureSensor, acMode);
ACControl acControl(buttonEnabled, infraredTransmitter, temperatureData);
// InfraredReceiver infraredReceiver(2);

WebServerHelper webServerHelper(buttonEnabled, temperatureSensor, infraredTransmitter, temperatureData, acMode);

void setup() {
  // fast baud for IR receiver
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  buttonEnabled.setup();
  temperatureSensor.setup();
  infraredTransmitter.setup();
  // infraredReceiver.setup();

  WifiHelper::setup();
  webServerHelper.setup();
}

void loop() {
  buttonEnabled.loop();
  temperatureSensor.loop();
  acControl.loop();
  // infraredReceiver.loop();

  webServerHelper.loop();
}
