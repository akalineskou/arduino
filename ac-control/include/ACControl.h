#pragma once

#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"

class ACControl {
  ButtonEnabled &buttonEnabled;
  InfraredTransmitter &infraredTransmitter;
  TemperatureData temperatureData;

public:
  ACControl(
    ButtonEnabled &buttonEnabled,
    InfraredTransmitter &infraredTransmitter,
    const TemperatureData &temperatureData
  );

  void loop() const;

private:
  void control() const;
};
