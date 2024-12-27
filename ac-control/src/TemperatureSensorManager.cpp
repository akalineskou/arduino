#include "TemperatureSensorManager.h"

TemperatureSensorManager::TemperatureSensorManager(
  TemperatureSensor **temperatureSensorsIn,
  TemperatureSensor **temperatureSensorsOut
): temperatureSensorsIn(temperatureSensorsIn),
   temperatureSensorsOut(temperatureSensorsOut) {
  countIn = calculateCount(temperatureSensorsIn);
  countOut = calculateCount(temperatureSensorsOut);
}

void TemperatureSensorManager::setup() {
  for (int i = 0; i < countIn; i++) {
    temperatureSensorsIn[i]->setup();
  }
  for (int i = 0; i < countOut; i++) {
    temperatureSensorsOut[i]->setup();
  }
}

void TemperatureSensorManager::loop() {
  for (int i = 0; i < countIn; i++) {
    temperatureSensorsIn[i]->loop();
  }
  for (int i = 0; i < countOut; i++) {
    temperatureSensorsOut[i]->loop();
  }
}

int TemperatureSensorManager::temperatureIn() const {
  auto sensorsIn = filterSensorsNotFailed(temperatureSensorsIn);
  const int sensorsInCount = calculateCount(sensorsIn);

  if (0 == sensorsInCount) {
    return 0;
  }

  // average
  int temperature = 0;

  for (int i = 0; i < sensorsInCount; i++) {
    temperature += sensorsIn[i]->temperature;
  }

  return temperature / sensorsInCount;
}

int TemperatureSensorManager::temperatureOut() const {
  auto sensorsOut = filterSensorsNotFailed(temperatureSensorsOut);
  const int sensorsOutCount = calculateCount(sensorsOut);

  if (0 == sensorsOutCount) {
    return 0;
  }

  // average
  int temperature = 0;

  for (int i = 0; i < sensorsOutCount; i++) {
    temperature += sensorsOut[i]->temperature;
  }

  return temperature / sensorsOutCount;
}

bool TemperatureSensorManager::sensorsInFailed() const {
  return 0 == calculateCount(filterSensorsNotFailed(temperatureSensorsIn));
}

bool TemperatureSensorManager::sensorsOutFailed() const {
  return 0 == calculateCount(filterSensorsNotFailed(temperatureSensorsOut));
}

TemperatureSensor **TemperatureSensorManager::filterSensorsNotFailed(TemperatureSensor **temperatureSensors) const {
  TemperatureSensor **temperatureSensorsNotFailed[countIn];
  int currentI = 0;

  for (int i = 0; i < countIn; i++) {
    if (temperatureSensors[i]->sensorFailed()) {
      continue;
    }

    temperatureSensorsNotFailed[currentI] = &temperatureSensors[i];
    currentI++;
  }

  return *temperatureSensorsNotFailed;
}

int TemperatureSensorManager::calculateCount(TemperatureSensor **temperatureSensors) const {
  return sizeof(*temperatureSensors) / sizeof(TemperatureSensor *) + 1;
}