#include "Directive.h"
#include "TemperatureData.h"

TemperatureData::TemperatureData(TemperatureSensor &temperatureSensor, ACMode &acMode):
    temperatureSensor(temperatureSensor),
    acMode(acMode) {
  temperatureTargetCold = 29.0 * 10;
  temperatureTargetHeat = 20.5 * 10;

  if (acMode == Cold) {
    temperatureTarget = temperatureTargetCold;
    temperatureStart = 1.0 * 10;
    temperatureStop = -0.5 * 10;
  } else {
    temperatureTarget = temperatureTargetHeat;
    temperatureStart = -1.0 * 10;
    temperatureStop = 0.5 * 10;
  }
}

int TemperatureData::getTemperature() const {
  return temperatureSensor.getTemperature();
}

int TemperatureData::getHumidity() const {
  return temperatureSensor.getHumidity();
}

int TemperatureData::temperatureStartReached(int temperatureCheck) const {
  bool temperatureStartReached;

  if (temperatureCheck == -1) {
    temperatureCheck = temperatureTargetStart();
  }

  if (acMode == Cold) {
    temperatureStartReached = getTemperature() >= temperatureCheck;
  } else {
    temperatureStartReached = getTemperature() <= temperatureCheck;
  }

  if (temperatureStartReached) {
#if APP_DEBUG
    Serial.printf(
      "Temperature start %s reached.\n",
      TemperatureSensor::formatTemperature(temperatureTargetStart()).c_str()
    );
#endif
  }

  return temperatureStartReached;
}

int TemperatureData::temperatureStopReached(int temperatureCheck) const {
  bool temperatureStopReached;

  if (temperatureCheck == -1) {
    temperatureCheck = temperatureTargetStop();
  }

  if (acMode == Cold) {
    temperatureStopReached = getTemperature() <= temperatureCheck;
  } else {
    temperatureStopReached = getTemperature() >= temperatureCheck;
  }

  if (temperatureStopReached) {
#if APP_DEBUG
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
