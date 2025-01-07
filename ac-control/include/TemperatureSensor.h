#pragma once

#include <DHT.h>
#include "millisDelay.h"

class TemperatureSensor {
  int pin;

  DHT dht;
  int sensorFails;
  millisDelay timeDelay;
  int temperature;
  int humidity;

public:
  explicit TemperatureSensor(int pin);

  void setup();

  void loop();

  bool sensorFailed() const;

  int getTemperature() const;

  int getHumidity() const;

  static String formatTemperature(int temperature);

  static String formatHumidity(int humidity);

private:
  void readTemperature(bool forceTimeDelay = false);
};
