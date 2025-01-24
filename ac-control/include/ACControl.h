#pragma once

#include "DatabaseHelper.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TimeDelay.h"

class ACControl {
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  DatabaseHelper &databaseHelper;
  TimeDelay turnOffTimeDelay;
  TimeDelay temperatureChangeTimeDelay;

 public:
  bool enabled;
  int temperatureStart;
  int temperatureStop;

  ACControl(InfraredTransmitter &infraredTransmitter, TemperatureData &temperatureData, DatabaseHelper &databaseHelper);

  void loop();

  void enable();

  void disable();

  void off() const;

  void start();

  void stop();

 private:
  void control();

  void databaseInsert(ACCommand acCommand) const;
};
