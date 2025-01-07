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

  bool isEnabled() const;

  void toggleStatus(bool triggerChange = true);

  void loop();

private:
  void control();
};
