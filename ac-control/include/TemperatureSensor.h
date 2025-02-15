#pragma once

#include <DHT.h>

#include "TimeDelay.h"

class TemperatureSensor {
  int pin;

  DHT dht;
  int sensorFails;
  TimeDelay temperatureTimeDelay;
  TimeDelay humidityTimeDelay;
  int temperature;
  int humidity;

 public:
  explicit TemperatureSensor(int pin);

  void setup();

  void loop();

  bool sensorFailed() const;

  int getTemperature() const;

  int getHumidity() const;

  static std::string formatTemperature(int temperature);

  static std::string formatHumidity(int humidity);

 private:
  void readTemperature(bool forceTimeDelay = false);

  void readHumidity(bool forceTimeDelay = false);
};
