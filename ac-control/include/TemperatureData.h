#pragma once

#include "ACMode.h"
#include "TemperatureSensorManager.h"

class TemperatureData {
  TemperatureSensorManager& temperatureSensorManager;
  ACMode acMode;

  int temperatureStart;
  int temperatureStop;

 public:
  int temperatureTarget;

  explicit TemperatureData(TemperatureSensorManager& temperatureSensorManager, const ACMode& acMode);

  int temperatureStartReached() const;

  int temperatureStopReached() const;

  int temperatureTargetStart() const;

  int temperatureTargetStop() const;

  bool temperatureSensorsInFailed() const;
};
