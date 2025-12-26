#pragma once

#include <DHT.h>

#include "DatabaseHelper.h"
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
  DatabaseHelper &databaseHelper;

  explicit TemperatureSensor(int pin, DatabaseHelper &databaseHelper);

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
