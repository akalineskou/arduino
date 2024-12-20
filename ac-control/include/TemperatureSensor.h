#pragma once

#include <DHT.h>
#include "ButtonEnabled.h"

class TemperatureSensor {
  int pin;
  ButtonEnabled &buttonEnabled;

  DHT dht;
  unsigned long lastCall = 0;

public:
  int temperature = 0;
  bool hasChanged = false;

  explicit TemperatureSensor(
    const int pin,
    ButtonEnabled &buttonEnabled
  ): pin(pin),
     buttonEnabled(buttonEnabled),
     dht(pin, DHT22) {
  }

  void setup();

  void loop();

private:
  void check();
};
