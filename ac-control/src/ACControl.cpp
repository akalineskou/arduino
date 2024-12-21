#include "ACControl.h"

void ACControl::loop() const {
  control();
}

void ACControl::control() const {
  if (!buttonEnabled.enabled) {
    if (buttonEnabled.hasChanged) {
      // turn off on button disable change (since it starts disabled, this means it was enabled then disabled)
      infraredTransmitter.sendCommand(Off, true);
    }

    return;
  }

  if (temperatureSensor.temperature >= temperatureTargetWithThresholdUp() && infraredTransmitter.lastACCommand != Stop) {
    Serial.printf("Temperature up threshold %.2f°C reached.\n", temperatureTargetWithThresholdUp() / 10.0);

    infraredTransmitter.sendCommand(Stop);
  } else if (temperatureSensor.temperature <= temperatureTargetWithThresholdDown() && infraredTransmitter.lastACCommand != Start) {
    Serial.printf("Temperature down threshold %.2f°C reached.\n", temperatureTargetWithThresholdDown() / 10.0);

    infraredTransmitter.sendCommand(Start);
  } else if (infraredTransmitter.lastACCommand != Stop) {
    // temperature is between the up and down threshold, turn on using Stop
    Serial.println("Turning on A/C");

    infraredTransmitter.sendCommand(Stop);
  }
}

int ACControl::temperatureTargetWithThresholdUp() const {
  return temperatureTarget + temperatureThresholdUp;
}

int ACControl::temperatureTargetWithThresholdDown() const {
  return temperatureTarget - temperatureThresholdDown;
}
