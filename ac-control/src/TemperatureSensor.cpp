#include "Directive.h"
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(
  const int pin
): pin(pin),
   dht(pin, DHT22),
   timeDelay(TimeDelay(2 * 1000)) {
  sensorFails = 0;
  temperature = 0;
  humidity = 0;
}

void TemperatureSensor::setup() {
  dht.begin();

  readTemperature(true);
}

void TemperatureSensor::loop() {
  readTemperature();
}

bool TemperatureSensor::sensorFailed() const {
  return sensorFails > 10;
}

int TemperatureSensor::getTemperature() const {
  return temperature;
}

int TemperatureSensor::getHumidity() const {
  return humidity;
}

String TemperatureSensor::formatTemperature(const int temperature) {
  char buffer[10];
  sprintf(buffer, "%.2f°C", temperature / 10.0);

  return {buffer};
}

String TemperatureSensor::formatHumidity(const int humidity) {
  char buffer[8];
  sprintf(buffer, "%.2f%%", humidity / 10.0);

  return {buffer};
}

void TemperatureSensor::readTemperature(const bool forceTimeDelay) {
  if (!timeDelay.finished(forceTimeDelay, true)) {
    return;
  }

  const float temperatureFloat = dht.readTemperature();
  if (isnan(temperatureFloat)) {
#if DEBUG
    Serial.printf("Failed to read temperature from sensor %d!\n", pin);
#endif

    sensorFails++;

    return;
  }

  // reset after getting a valid temperature
  sensorFails = 0;

  const int temperatureInt = static_cast<int>(round(temperatureFloat * 10));
  if (temperatureInt == temperature) {
    return;
  }

  const int humidityInt = static_cast<int>(round(dht.readHumidity() * 10));

#if DEBUG && DEBUG_TEMPERATURE_CHANGE
  const int temperatureDiff = temperatureInt - temperature;

  Serial.printf(
    "Sensor %d temperature change: %s (%s%s), humidity: %s.\n", pin, formatTemperature(temperature).c_str(),
    temperatureDiff < 0 ? "" : "+", formatTemperature(temperatureDiff).c_str(), formatHumidity(humidityInt).c_str());
#endif

  temperature = temperatureInt;
  humidity = humidityInt;
}
