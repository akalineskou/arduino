#pragma once

#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "TemperatureSensor.h"

class ACControl {
  ButtonEnabled &buttonEnabled;
  TemperatureSensor &temperatureSensor;
  InfraredTransmitter &infraredTransmitter;

  int temperatureTarget = 20.5 * 10;
  int temperatureThresholdUp = 0.5 * 10;
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
  void control() const;

  int temperatureTargetWithThresholdUp() const;

  int temperatureTargetWithThresholdDown() const;
};
