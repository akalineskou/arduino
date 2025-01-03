#pragma once

#include <DHT.h>

class TemperatureSensor {
  int pin;

  DHT dht;
  unsigned long lastCall;
  int sensorFails;

public:
  int temperature;
  bool hasChanged;

  explicit TemperatureSensor(int pin);

  void setup();

  void loop();

  bool sensorFailed() const;

  static String formatTemperature(int temperature);

private:
  void readTemperature(bool forceSend = false);
};
