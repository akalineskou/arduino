#include "ACControl.h"
#include "ACData.h"
#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "TemperatureSensor.h"
// #include "InfraredReceiver.h"

ButtonEnabled buttonEnabled(4);
TemperatureSensor temperatureSensor(0);
InfraredTransmitter infraredTransmitter(16, ACData());
ACControl acControl(buttonEnabled, temperatureSensor, infraredTransmitter);
// InfraredReceiver infraredReceiver(2);

void setup() {
  delay(150);

  // fast baud for IR receiver
  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  temperatureSensor.setup();
  buttonEnabled.setup();
  infraredTransmitter.setup();
  // infraredReceiver.setup();
}

void loop() {
  buttonEnabled.loop();
  temperatureSensor.loop();
  acControl.loop();
  // infraredReceiver.loop();
}
