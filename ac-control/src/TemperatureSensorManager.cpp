#include "TemperatureSensorManager.h"

TemperatureSensorManager::TemperatureSensorManager(
  TemperatureSensor **temperatureSensorsIn,
  const int countIn,
  TemperatureSensor **temperatureSensorsOut,
  const int countOut
): temperatureSensorsIn(temperatureSensorsIn),
   countIn(countIn),
   temperatureSensorsOut(temperatureSensorsOut),
   countOut(countOut) {
}

void TemperatureSensorManager::setup() const {
  for (int i = 0; i < countIn; i++) {
    temperatureSensorsIn[i]->setup();
  }
  for (int i = 0; i < countOut; i++) {
    temperatureSensorsOut[i]->setup();
  }
}

void TemperatureSensorManager::loop() const {
  for (int i = 0; i < countIn; i++) {
    temperatureSensorsIn[i]->loop();
  }
  for (int i = 0; i < countOut; i++) {
    temperatureSensorsOut[i]->loop();
  }
}

int TemperatureSensorManager::temperatureIn() const {
  const auto sensorsInWithCount = filterSensorsNotFailed(temperatureSensorsIn, countIn);

  if (0 == sensorsInWithCount.count) {
    return 0;
  }

  // average
  int temperature = 0;

  for (int i = 0; i < sensorsInWithCount.count; i++) {
    temperature += sensorsInWithCount.temperatureSensors[i]->temperature;
  }

  return temperature / sensorsInWithCount.count;
}

int TemperatureSensorManager::temperatureOut() const {
  const auto sensorsOutWithCount = filterSensorsNotFailed(temperatureSensorsOut, countOut);

  if (0 == sensorsOutWithCount.count) {
    return 0;
  }

  // average
  int temperature = 0;

  for (int i = 0; i < sensorsOutWithCount.count; i++) {
    temperature += sensorsOutWithCount.temperatureSensors[i]->temperature;
  }

  return temperature / sensorsOutWithCount.count;
}

int TemperatureSensorManager::humidityIn() const {
  const auto sensorsInWithCount = filterSensorsNotFailed(temperatureSensorsIn, countIn);

  if (0 == sensorsInWithCount.count) {
    return 0;
  }

  // average
  int humidity = 0;

  for (int i = 0; i < sensorsInWithCount.count; i++) {
    humidity += sensorsInWithCount.temperatureSensors[i]->humidity;
  }

  return humidity / sensorsInWithCount.count;
}

int TemperatureSensorManager::humidityOut() const {
  const auto sensorsOutWithCount = filterSensorsNotFailed(temperatureSensorsOut, countOut);

  if (0 == sensorsOutWithCount.count) {
    return 0;
  }

  // average
  int humidity = 0;

  for (int i = 0; i < sensorsOutWithCount.count; i++) {
    humidity += sensorsOutWithCount.temperatureSensors[i]->humidity;
  }

  return humidity / sensorsOutWithCount.count;
}

bool TemperatureSensorManager::sensorsInFailed() const {
  return 0 == filterSensorsNotFailed(temperatureSensorsIn, countIn).count;
}

bool TemperatureSensorManager::sensorsOutFailed() const {
  return 0 == filterSensorsNotFailed(temperatureSensorsOut, countOut).count;
}

TemperatureSensorsWithCount TemperatureSensorManager::filterSensorsNotFailed(TemperatureSensor **temperatureSensors, const int maxCount) {
  TemperatureSensor **temperatureSensorsNotFailed[maxCount];
  int currentI = 0;

  for (int i = 0; i < maxCount; i++) {
    if (temperatureSensors[i]->sensorFailed()) {
      continue;
    }

    temperatureSensorsNotFailed[currentI] = &temperatureSensors[i];
    currentI++;
  }

  TemperatureSensorsWithCount temperatureSensorsWithCount{};
  temperatureSensorsWithCount.temperatureSensors = *temperatureSensorsNotFailed;
  temperatureSensorsWithCount.count = currentI;

  return temperatureSensorsWithCount;
}
