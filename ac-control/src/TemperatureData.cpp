#include "Directive.h"
#include "TemperatureData.h"

TemperatureData::TemperatureData(TemperatureSensor &temperatureSensor, const ACMode &acMode):
    temperatureSensor(temperatureSensor),
    acMode(acMode) {
  if (acMode == Cold) {
    temperatureTarget = 29.0 * 10;
    temperatureStart = 1.0 * 10;
    temperatureStop = -0.5 * 10;
  } else {
    temperatureTarget = 21.0 * 10;
    temperatureStart = -1.0 * 10;
    temperatureStop = 0.5 * 10;
  }
}

int TemperatureData::temperatureStartReached() const {
  bool temperatureStartReached;

  if (acMode == Cold) {
    temperatureStartReached = temperatureSensor.getTemperature() > temperatureTargetStart();
  } else {
    temperatureStartReached = temperatureSensor.getTemperature() < temperatureTargetStart();
  }

  if (temperatureStartReached) {
#if DEBUG
    Serial.printf(
      "Temperature start %s reached.\n",
      TemperatureSensor::formatTemperature(temperatureTargetStart()).c_str()
    );
#endif
  }

  return temperatureStartReached;
}

int TemperatureData::temperatureStopReached() const {
  bool temperatureStopReached;

  if (acMode == Cold) {
    temperatureStopReached = temperatureSensor.getTemperature() < temperatureTargetStop();
  } else {
    temperatureStopReached = temperatureSensor.getTemperature() > temperatureTargetStop();
  }

  if (temperatureStopReached) {
#if DEBUG
    Serial.printf(
      "Temperature stop %s reached.\n",
      TemperatureSensor::formatTemperature(temperatureTargetStop()).c_str()
    );
#endif
  }

  return temperatureStopReached;
}

int TemperatureData::temperatureTargetStart() const {
  return temperatureTarget + temperatureStart;
}

int TemperatureData::temperatureTargetStop() const {
  return temperatureTarget + temperatureStop;
}

bool TemperatureData::temperatureSensorFailed() const {
  return temperatureSensor.sensorFailed();
}
