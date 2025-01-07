#pragma once

#include "InfraredTransmitter.h"
#include "TemperatureData.h"

class ACControl {
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  bool enabled;
  bool wasChanged;

public:
  ACControl(
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData
  );

  void setup() const;

  void loop();

  bool isEnabled() const;

  void toggleStatus(bool triggerChange = true);

private:
  void control();
};
