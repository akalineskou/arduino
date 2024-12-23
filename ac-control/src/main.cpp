#include "ACControl.h"
#include "ACMode.h"
#include "ButtonEnabled.h"
// #include "InfraredReceiver.h"
#include "InfraredTransmitter.h"
#include "IRData.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"
#include "WebServerHelper.h"
#include "WebServerTemplate.h"
#include "WifiHelper.h"

ACMode acMode(Heat);
ButtonEnabled buttonEnabled(4);
TemperatureSensor temperatureSensor(0);
InfraredTransmitter infraredTransmitter(16, IRData(acMode));
ACControl acControl(buttonEnabled, infraredTransmitter, TemperatureData(temperatureSensor, acMode));
// InfraredReceiver infraredReceiver(2);

WifiHelper wifiHelper;
WebServerTemplate webServerTemplate(temperatureSensor);
WebServerHelper webServerHelper(webServerTemplate);


void setup() {
  // fast baud for IR receiver
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  delay(2500);

  buttonEnabled.setup();
  temperatureSensor.setup();
  infraredTransmitter.setup();
  // infraredReceiver.setup();

  wifiHelper.setup();
  webServerHelper.setup();
}

void loop() {
  buttonEnabled.loop();
  temperatureSensor.loop();
  acControl.loop();
  // infraredReceiver.loop();

  webServerHelper.loop();
}
