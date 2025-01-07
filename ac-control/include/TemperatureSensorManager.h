#pragma once

#include "TemperatureSensor.h"

struct TemperatureSensorsWithCount {
  TemperatureSensor** temperatureSensors;
  int count;
};

class TemperatureSensorManager {
  TemperatureSensor** temperatureSensors;
  int count;

 public:
  explicit TemperatureSensorManager(TemperatureSensor** temperatureSensors, int count);

  void setup() const;

  void loop() const;

  int temperature() const;

  int humidity() const;

  bool sensorsFailed() const;

 private:
  static TemperatureSensorsWithCount filterSensorsNotFailed(TemperatureSensor** temperatureSensors, int maxCount);
};
