#include "Directive.h"
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(const int pin):
    pin(pin),
    dht(pin, DHT_TYPE),
    temperatureTimeDelay(TimeDelay(APP_TEMPERATURE_SENSOR_TEMPERATURE_DELAY, true)),
    humidityTimeDelay(TimeDelay(APP_TEMPERATURE_SENSOR_HUMIDITY_DELAY, true)) {
  sensorFails = 0;
  temperature = 0;
  humidity = 0;

#if APP_DEBUG && APP_DEBUG_RANDOM_TEMPERATURE_SENSOR
  randomSeed(analogRead(0));
#endif
}

void TemperatureSensor::setup() {
  dht.begin();

  readTemperature(true);
  readHumidity(true);
}

void TemperatureSensor::loop() {
  readTemperature();
  readHumidity();
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

std::string TemperatureSensor::formatTemperature(const int temperature) {
  char buffer[10];
  sprintf(buffer, "%.2f°C", temperature / 10.0);

  return {buffer};
}

std::string TemperatureSensor::formatHumidity(const int humidity) {
  char buffer[8];
  sprintf(buffer, "%.2f%%", humidity / 10.0);

  return {buffer};
}

void TemperatureSensor::readTemperature(const bool forceTimeDelay) {
  if (!temperatureTimeDelay.delayPassed(forceTimeDelay)) {
    return;
  }

#if APP_DEBUG && APP_DEBUG_RANDOM_TEMPERATURE_SENSOR
  temperature = random(190, 220);

  return;
#endif

  const float temperatureFloat = dht.readTemperature();
  if (isnan(temperatureFloat)) {
#if APP_DEBUG
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

#if APP_DEBUG && APP_DEBUG_TEMPERATURE_CHANGE
  const int temperatureDiff = temperatureInt - temperature;

  Serial.printf(
    "Sensor %d temperature change: %s (%s%s).\n",
    pin,
    formatTemperature(temperature).c_str(),
    temperatureDiff < 0 ? "" : "+",
    formatTemperature(temperatureDiff).c_str()
  );
#endif

  temperature = temperatureInt;
}

void TemperatureSensor::readHumidity(const bool forceTimeDelay) {
  if (!humidityTimeDelay.delayPassed(forceTimeDelay)) {
    return;
  }

#if APP_DEBUG && APP_DEBUG_RANDOM_TEMPERATURE_SENSOR
  humidity = random(550, 900);

  return;
#endif

  const float humidityFloat = dht.readHumidity();
  if (isnan(humidityFloat)) {
#if APP_DEBUG
    Serial.printf("Failed to read humidity from sensor %d!\n", pin);
#endif
    return;
  }

  const int humidityInt = static_cast<int>(round(humidityFloat * 10));
  if (humidityInt == humidity) {
    return;
  }

#if APP_DEBUG && APP_DEBUG_TEMPERATURE_CHANGE
  const int humidityDiff = humidityInt - humidity;

  Serial.printf(
    "Sensor %d humidity change: humidity: %s (%s%s).\n",
    pin,
    formathumidity(humidity).c_str(),
    humidityDiff < 0 ? "" : "+",
    formathumidity(humidityDiff).c_str()
  );
#endif

  humidity = humidityInt;
}
