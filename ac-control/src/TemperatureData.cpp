#include "Directive.h"
#include "Preference.h"
#include "TemperatureData.h"

TemperatureData::TemperatureData(TemperatureSensor &temperatureSensor, DatabaseHelper &databaseHelper, ACMode &acMode):
    databaseHelper(databaseHelper),
    acMode(acMode),
    temperatureSensor(temperatureSensor) {
  temperatureTargetCold = 29.0 * 10;
  temperatureTargetHeat = 20.5 * 10;
  temperatureStart = -1;
  temperatureStop = -1;

  temperatureTarget = -1;
}

void TemperatureData::loop() {
  updateTemperatures();
}

void TemperatureData::updateTemperatures(const bool force) {
  if (temperatureTarget == -1 || force) {
    if (acMode == Cold) {
      temperatureTarget = temperatureTargetCold;
    } else {
      temperatureTarget = temperatureTargetHeat;
    }
  }

  temperatureStart = 0.5 * 10;
  temperatureStop = 0.5 * 10;

  if (acMode == Cold) {
    temperatureStop *= -1;
  } else {
    temperatureStart *= -1;
  }
}

int TemperatureData::getTemperature() const {
  return temperatureSensor.getTemperature();
}

int TemperatureData::getHumidity() const {
  return temperatureSensor.getHumidity();
}

int TemperatureData::temperatureStartReached(int temperatureCheck, const bool equalCheck) const {
  bool temperatureStartReached;

  if (temperatureCheck == -1) {
    temperatureCheck = temperatureTargetStart();
  }

  if (acMode == Cold) {
    if (equalCheck) {
      temperatureStartReached = getTemperature() >= temperatureCheck;
    } else {
      temperatureStartReached = getTemperature() > temperatureCheck;
    }
  } else {
    if (equalCheck) {
      temperatureStartReached = getTemperature() <= temperatureCheck;
    } else {
      temperatureStartReached = getTemperature() < temperatureCheck;
    }
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

int TemperatureData::temperatureStopReached(int temperatureCheck, const bool equalCheck) const {
  bool temperatureStopReached;

  if (temperatureCheck == -1) {
    temperatureCheck = temperatureTargetStop();
  }

  if (acMode == Cold) {
    if (equalCheck) {
      temperatureStopReached = getTemperature() <= temperatureCheck;
    } else {
      temperatureStopReached = getTemperature() < temperatureCheck;
    }
  } else {
    if (equalCheck) {
      temperatureStopReached = getTemperature() >= temperatureCheck;
    } else {
      temperatureStopReached = getTemperature() > temperatureCheck;
    }
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

void TemperatureData::temperatureTargetIncrease() {
  temperatureTarget += 0.5 * 10;

  databaseHelper.updatePreferenceByType(TdTemperatureTarget, reinterpret_cast<void*>(temperatureTarget));
}

void TemperatureData::temperatureTargetDecrease() {
  temperatureTarget -= 0.5 * 10;

  databaseHelper.updatePreferenceByType(TdTemperatureTarget, reinterpret_cast<void*>(temperatureTarget));
}

bool TemperatureData::temperatureSensorFailed() const {
  return temperatureSensor.sensorFailed();
}
