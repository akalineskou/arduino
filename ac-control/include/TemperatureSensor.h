#pragma once

#include <DHT.h>
#include "millisDelay.h"

class TemperatureSensor {
  int pin;

  DHT dht;
  int sensorFails;
  millisDelay timeDelay;

public:
  int temperature;
  int humidity;
  bool hasChanged;

  explicit TemperatureSensor(int pin);

  void setup();

  void loop();

  bool sensorFailed() const;

  static String formatTemperature(int temperature);

  static String formatHumidity(int humidity);

private:
  void readTemperature(bool forceSend = false);
};
