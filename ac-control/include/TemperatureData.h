#pragma once

#include "ACMode.h"
#include "TemperatureSensor.h"

class TemperatureData {
  TemperatureSensor &temperatureSensor;
  ACMode &acMode;

  int temperatureStart;
  int temperatureStop;

 public:
  int temperatureTarget;
  int temperatureTargetCold;
  int temperatureTargetHeat;

  explicit TemperatureData(TemperatureSensor &temperatureSensor, ACMode &acMode);

  int getTemperature() const;

  int getHumidity() const;

  int temperatureStartReached(int temperatureCheck = -1) const;

  int temperatureStopReached(int temperatureCheck = -1) const;

  int temperatureTargetStart() const;

  int temperatureTargetStop() const;

  bool temperatureSensorFailed() const;
};
