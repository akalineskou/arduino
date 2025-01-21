#pragma once

#include "DatabaseHelper.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TimeDelay.h"

class ACControl {
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  DatabaseHelper &databaseHelper;

  bool enabled;
  TimeDelay turnOffTimeDelay;

 public:
  ACControl(InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData, DatabaseHelper &databaseHelper);

  void loop();

  void enable();

  void disable();

  bool isEnabled() const;

  void off() const;

  void start();

  void stop();

 private:
  void control();

  void databaseInsert(ACCommand acCommand) const;
};
