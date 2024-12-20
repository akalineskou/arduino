#pragma once

#include <DHT.h>

class TemperatureSensor {
  int pin;

  DHT dht;
  unsigned long lastCall = 0;

public:
  int temperature = 0;
  bool hasChanged = false;

  explicit TemperatureSensor(
    const int pin
  ): pin(pin),
     dht(pin, DHT22) {
  }

  void setup();

  void loop();

private:
  void check();
};
