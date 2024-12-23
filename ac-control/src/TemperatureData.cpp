#include <Arduino.h>
#include "TemperatureData.h"

TemperatureData::TemperatureData(
  TemperatureSensor &temperatureSensor,
  const ACMode &acMode
): temperatureSensor(temperatureSensor),
   acMode(acMode) {
  if (acMode == Cold) {
    temperatureTarget = 29.0 * 10;
    temperatureStart = 1.0 * 10;
    temperatureStop = -0.5 * 10;
  } else {
    temperatureTarget = 20.5 * 10;
    temperatureStart = -1.0 * 10;
    temperatureStop = 0.5 * 10;
  }
}

int TemperatureData::temperatureStartReached() const {
  bool temperatureStartReached;

  if (acMode == Cold) {
    temperatureStartReached = temperatureSensor.temperature > temperatureTargetStart();
  } else {
    temperatureStartReached = temperatureSensor.temperature < temperatureTargetStart();
  }

  if (temperatureStartReached) {
    Serial.printf("Temperature start %s reached.\n", TemperatureSensor::formatTemperature(temperatureTargetStart()).c_str());
  }

  return temperatureStartReached;
}

int TemperatureData::temperatureStopReached() const {
  bool temperatureStopReached;

  if (acMode == Cold) {
    temperatureStopReached = temperatureSensor.temperature < temperatureTargetStop();
  } else {
    temperatureStopReached = temperatureSensor.temperature > temperatureTargetStop();
  }

  if (temperatureStopReached) {
    Serial.printf("Temperature stop %s reached.\n", TemperatureSensor::formatTemperature(temperatureTargetStop()).c_str());
  }

  return temperatureStopReached;
}

int TemperatureData::temperatureTargetStart() const {
  return temperatureTarget + temperatureStart;
}

int TemperatureData::temperatureTargetStop() const {
  return temperatureTarget + temperatureStop;
}
