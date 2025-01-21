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

int TemperatureData::temperatureStartReached() const {
  bool temperatureStartReached;

  if (acMode == Cold) {
    temperatureStartReached = getTemperature() > temperatureTargetStart();
  } else {
    temperatureStartReached = getTemperature() < temperatureTargetStart();
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

int TemperatureData::temperatureStopReached() const {
  bool temperatureStopReached;

  if (acMode == Cold) {
    temperatureStopReached = getTemperature() < temperatureTargetStop();
  } else {
    temperatureStopReached = getTemperature() > temperatureTargetStop();
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
