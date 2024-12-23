#include <Arduino.h>
#include "TemperatureData.h"
#include "TemperatureSensor.h"

TemperatureData::TemperatureData(
  TemperatureSensor &temperatureSensor,
  const ACMode &acMode
): temperatureSensor(temperatureSensor),
   acMode(acMode) {
  if (acMode == Cold) {
    temperatureTarget = 29.0 * 10;
    temperatureMin = 0.5 * 10;
    temperatureMax = 1.0 * 10;
  } else {
    temperatureTarget = 20.5 * 10;
    temperatureMin = 1.0 * 10;
    temperatureMax = 0.5 * 10;
  }
}

int TemperatureData::temperatureStart() const {
  bool temperatureStart;

  if (acMode == Cold) {
    temperatureStart = temperatureSensor.temperature > temperatureTargetStart();
  } else {
    temperatureStart = temperatureSensor.temperature < temperatureTargetStart();
  }

  if (temperatureStart) {
    Serial.printf("Temperature start %s reached.\n", TemperatureSensor::formatTemperature(temperatureTargetStart()).c_str());
  }

  return temperatureStart;
}

int TemperatureData::temperatureStop() const {
  bool temperatureStop;

  if (acMode == Cold) {
    temperatureStop = temperatureSensor.temperature < temperatureTargetStop();
  } else {
    temperatureStop = temperatureSensor.temperature > temperatureTargetStop();
  }

  if (temperatureStop) {
    Serial.printf("Temperature stop %s reached.\n", TemperatureSensor::formatTemperature(temperatureTargetStop()).c_str());
  }

  return temperatureStop;
}

int TemperatureData::temperatureTargetStart() const {
  return temperatureTarget + acMode == Cold ? temperatureMax : -temperatureMin;
}

int TemperatureData::temperatureTargetStop() const {
  return temperatureTarget + acMode == Cold ? -temperatureMin : temperatureMax;
}
