#pragma once

#include "InfraredTransmitter.h"
#include "TemperatureData.h"

class ACControl {
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;

public:
  bool enabled;

  ACControl(
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData
  );

  void loop();

  void enable();

  void disable();

private:
  void control();
};
