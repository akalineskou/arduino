#include "TemperatureSensor.h"

void TemperatureSensor::setup() {
  dht.begin();
}

void TemperatureSensor::loop() {
  check();

  if (hasChanged) {
    Serial.printf("Sensor %d temperature change: %.2f°C\n", pin, temperature / 10.0);
  }
}

void TemperatureSensor::check() {
  hasChanged = false;

  if (millis() - lastCall < 2 * 1000) {
    // get temp readings every 2 seconds
    return;
  }
  lastCall = millis();

  const float tempFloat = dht.readTemperature();
  if (isnan(tempFloat)) {
    Serial.printf("Failed to read temperature from sensor %d!\n", pin);

    return;
  }

  const int tempInt = static_cast<int>(round(tempFloat * 10));
  if (tempInt == temperature) {
    return;
  }
  temperature = tempInt;

  hasChanged = true;
}
