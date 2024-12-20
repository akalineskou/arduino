#include "ACControl.h"
#include "ACData.h"
#include "ButtonEnabled.h"
#include "InfraredReceiver.h"
#include "InfraredTransmitter.h"
#include "TemperatureSensor.h"

ButtonEnabled buttonEnabled(4);
TemperatureSensor temperatureSensor(0, buttonEnabled);
InfraredTransmitter infraredTransmitter(16, ACData());
ACControl acControl(buttonEnabled, temperatureSensor, infraredTransmitter);
InfraredReceiver infraredReceiver(2);

void setup() {
  Serial.begin(115200); // fast baud

  temperatureSensor.setup();
  buttonEnabled.setup();
  infraredTransmitter.setup();
  infraredReceiver.setup();
}

void loop() {
  buttonEnabled.loop();
  temperatureSensor.loop();
  acControl.loop();
  infraredReceiver.loop();
}
