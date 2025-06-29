#pragma once

#include "ACMode.h"
#include "DatabaseHelper.h"
#include "TemperatureSensor.h"

class TemperatureData {
  TemperatureSensor &temperatureSensor;
  DatabaseHelper &databaseHelper;
  ACMode &acMode;

  int temperatureTargetCold;
  int temperatureTargetHeat;
  int temperatureStart;
  int temperatureStop;

 public:
  int temperatureTarget;

  explicit TemperatureData(TemperatureSensor &temperatureSensor, DatabaseHelper &databaseHelper, ACMode &acMode);

  void loop();

  void updateTemperatures(bool force = false);

  int getTemperature() const;

  int getHumidity() const;

  int temperatureStartReached(int temperatureCheck = -1, bool equalCheck = true) const;

  int temperatureStopReached(int temperatureCheck = -1, bool equalCheck = true) const;

  int temperatureTargetStart() const;

  int temperatureTargetStop() const;

  void temperatureTargetIncrease();

  void temperatureTargetDecrease();

  bool temperatureSensorFailed() const;
};
