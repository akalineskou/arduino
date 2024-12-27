#pragma once

#include "TemperatureSensor.h"

class TemperatureSensorManager {
  TemperatureSensor **temperatureSensorsIn;
  TemperatureSensor **temperatureSensorsOut;
  int countIn;
  int countOut;

public:
  explicit TemperatureSensorManager(
    TemperatureSensor **temperatureSensorsIn,
    TemperatureSensor **temperatureSensorsOut
  );

  void setup();

  void loop();

  int temperatureIn() const;

  int temperatureOut() const;

  bool sensorsInFailed() const;

  bool sensorsOutFailed() const;

private:
  TemperatureSensor **filterSensorsNotFailed(TemperatureSensor **temperatureSensors) const;

  int calculateCount(TemperatureSensor **temperatureSensors) const;
};
