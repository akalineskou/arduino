#pragma once

#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "TemperatureSensor.h"

class ACControl {
  ButtonEnabled &buttonEnabled;
  TemperatureSensor &temperatureSensor;
  InfraredTransmitter &infraredTransmitter;

  int temperatureTarget = 21.0 * 10;
  int temperatureThresholdUp = 1.0 * 10;
  int temperatureThresholdDown = 0.5 * 10;

public:
  ACControl(
    ButtonEnabled &buttonEnabled,
    TemperatureSensor &temperatureSensor,
    InfraredTransmitter &infraredTransmitter
  ): buttonEnabled(buttonEnabled),
     temperatureSensor(temperatureSensor),
     infraredTransmitter(infraredTransmitter) {
  }

  void loop() const;

private:
  int temperatureTargetWithThresholdUp() const;

  int temperatureTargetWithThresholdDown() const;
};
