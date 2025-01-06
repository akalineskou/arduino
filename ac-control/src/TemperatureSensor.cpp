#include "Serial.h"
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(
  const int pin
): pin(pin),
   dht(pin, DHT22),
   timeDelay(millisDelay()) {
  sensorFails = 0;
  temperature = 0;
  hasChanged = false;
}

void TemperatureSensor::setup() {
  dht.begin();

  readTemperature(true);

  D_printf("Sensor %d temperature: %s.\n", pin, formatTemperature(temperature).c_str());

  // get temp readings every 2s
  timeDelay.start(2 * 1000);
}

void TemperatureSensor::loop() {
  readTemperature();

  if (hasChanged) {
    D_printf("Sensor %d temperature change: %s.\n", pin, formatTemperature(temperature).c_str());
  }
}

bool TemperatureSensor::sensorFailed() const {
  return sensorFails > 10;
}

String TemperatureSensor::formatTemperature(const int temperature) {
  char buffer[10];
  sprintf(buffer, "%.2f°C", temperature / 10.0);

  return {buffer};
}

void TemperatureSensor::readTemperature(const bool forceSend) {
  hasChanged = false;

  if (!timeDelay.justFinished() && !forceSend) {
    return;
  }
  timeDelay.repeat();

  const float temperatureFloat = dht.readTemperature();
  if (isnan(temperatureFloat)) {
    D_printf("Failed to read temperature from sensor %d!\n", pin);

    sensorFails++;

    return;
  }

  // reset after getting a valid temperature
  sensorFails = 0;

  const int temperatureInt = static_cast<int>(round(temperatureFloat * 10));
  if (temperatureInt == temperature) {
    return;
  }

  temperature = temperatureInt;

  hasChanged = true;
}
