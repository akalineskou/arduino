#pragma once

#include "ACMode.h"
#include "TemperatureSensor.h"

class TemperatureData {
  TemperatureSensor &temperatureSensor;
  ACMode acMode;

  int temperatureStart;
  int temperatureStop;

public:
  int temperatureTarget;

  explicit TemperatureData(
    TemperatureSensor &temperatureSensor,
    const ACMode &acMode
  );

  int temperatureStartReached() const;

  int temperatureStopReached() const;

  int temperatureTargetStart() const;

  int temperatureTargetStop() const;
};
