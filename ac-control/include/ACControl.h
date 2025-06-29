#pragma once

#include "DatabaseHelper.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TimeDelay.h"

class ACControl {
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  DatabaseHelper &databaseHelper;
  ACMode &acMode;

  TimeDelay turnOffTimeDelay;
  TimeDelay temperatureChangeTimeDelay;

 public:
  bool enabled;
  int temperatureStart;
  int temperatureStop;
  bool spamOff;

  ACControl(
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData,
    DatabaseHelper &databaseHelper,
    ACMode &acMode
  );

  void loop();

  void enable();

  void disable();

  void off() const;

  void start();

  void stop();

  void changeMode();

  ACCommand lastACCommand() const;

 private:
  void control();

  void databaseInsert(ACCommand acCommand) const;
};
