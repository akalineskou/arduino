#include "TemperatureSensorManager.h"

TemperatureSensorManager::TemperatureSensorManager(TemperatureSensor** temperatureSensors, const int count)
    : temperatureSensors(temperatureSensors), count(count) {}

void TemperatureSensorManager::setup() const {
  for (int i = 0; i < count; i++) {
    temperatureSensors[i]->setup();
  }
}

void TemperatureSensorManager::loop() const {
  for (int i = 0; i < count; i++) {
    temperatureSensors[i]->loop();
  }
}

int TemperatureSensorManager::temperature() const {
  const auto sensorsWithCount = filterSensorsNotFailed(temperatureSensors, count);

  if (0 == sensorsWithCount.count) {
    return 0;
  }

  // average
  int temperature = 0;

  for (int i = 0; i < sensorsWithCount.count; i++) {
    temperature += sensorsWithCount.temperatureSensors[i]->getTemperature();
  }

  return temperature / sensorsWithCount.count;
}

int TemperatureSensorManager::humidity() const {
  const auto sensorsWithCount = filterSensorsNotFailed(temperatureSensors, count);

  if (0 == sensorsWithCount.count) {
    return 0;
  }

  // average
  int humidity = 0;

  for (int i = 0; i < sensorsWithCount.count; i++) {
    humidity += sensorsWithCount.temperatureSensors[i]->getHumidity();
  }

  return humidity / sensorsWithCount.count;
}

bool TemperatureSensorManager::sensorsFailed() const {
  return 0 == filterSensorsNotFailed(temperatureSensors, count).count;
}

TemperatureSensorsWithCount TemperatureSensorManager::filterSensorsNotFailed(
  TemperatureSensor** temperatureSensors, const int maxCount) {
  TemperatureSensor** temperatureSensorsNotFailed[maxCount];
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
