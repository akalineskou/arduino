#pragma once

#include "TemperatureSensor.h"

struct TemperatureSensorsWithCount {
  TemperatureSensor **temperatureSensors;
  int count;
};

class TemperatureSensorManager {
  TemperatureSensor **temperatureSensorsIn;
  int countIn;
  TemperatureSensor **temperatureSensorsOut;
  int countOut;

public:
  explicit TemperatureSensorManager(
    TemperatureSensor **temperatureSensorsIn,
    int countIn,
    TemperatureSensor **temperatureSensorsOut,
    int countOut
  );

  void setup() const;

  void loop() const;

  int temperatureIn() const;

  int temperatureOut() const;

  int humidityIn() const;

  int humidityOut() const;

  bool sensorsInFailed() const;

  bool sensorsOutFailed() const;

private:
  static TemperatureSensorsWithCount filterSensorsNotFailed(TemperatureSensor **temperatureSensors, int maxCount);
};
