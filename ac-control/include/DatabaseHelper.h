#pragma once

#include <sqlite3.h>

#include "Directive.h"
#include "TimeHelper.h"

struct TemperatureReading {
  int id = 0;
  int temperature = 0;
  int temperatureTargetStart = 0;
  int temperatureTargetStop = 0;
  int humidity = 0;
  long int time = 0;
};

struct TemperatureReadings {
  int numRows = 0;
  TemperatureReading* temperatureReadings = nullptr;
};

struct Command {
  int id = 0;
  std::string command{};
  int temperature = 0;
  int temperature_target = 0;
  long int time = 0;
};

struct Commands {
  int numRows = 0;
  Command* commands = nullptr;
};

struct Preference {
  bool acEnabled = false;
  std::string acMode{};
  std::string irLastACCommand{};
  bool irLightToggled = false;
  int tdTemperatureTarget = 0;
};

class DatabaseHelper {
  TimeHelper &timeHelper;

  sqlite3* database;
  sqlite3_stmt* statement;
  std::string sql{};

 public:
  explicit DatabaseHelper(TimeHelper &timeHelper);

  bool setup();

  void insertTemperatureReading(
    int temperature, int temperatureTargetStart, int temperatureTargetStop, int humidity, bool force = false
  );

  TemperatureReadings* selectTemperatureReadings(int hours);

  TemperatureReadings* selectTemperatureReadings(int everyMinutes, int hours);

  void insertCommand(const char* command, int temperature, int temperature_target);

  Commands* selectCommands(int maxRows = 5);

  Preference* selectPreference();

  void updatePreferenceAcEnabled(bool acEnabled);

  void updatePreferenceAcMode(const char* acMode);

  void updatePreferenceIrLastACCommand(const char* irLastACCommand);

  void updatePreferenceIrLightToggled(bool irLightToggled);

  void updatePreferenceTdTemperatureTarget(int tdTemperatureTarget);

 private:
  int prepare();

#if APP_DEBUG
  void expandSql() const;
#endif
};
