#pragma once

#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TimeDelay.h"

class ACControl {
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;

  bool enabled;
  TimeDelay turnOffTimeDelay;

 public:
  ACControl(InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData);

  void loop();

  void enable();

  void disable();

  bool isEnabled() const;

  void start(bool force = false);

  void stop(bool force = false);

 private:
  void control();
};
