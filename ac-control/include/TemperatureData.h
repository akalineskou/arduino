#pragma once

#include "ACMode.h"
#include "TemperatureSensor.h"

class TemperatureData {
  TemperatureSensor &temperatureSensor;
  ACMode acMode;

public:
  int temperatureTarget;
  int temperatureMin;
  int temperatureMax;

  explicit TemperatureData(
    TemperatureSensor &temperatureSensor,
    const ACMode &acMode
  );

  int temperatureStart() const;

  int temperatureStop() const;

private:
  int temperatureTargetStart() const;

  int temperatureTargetStop() const;
};
