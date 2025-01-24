#pragma once

#include <sqlite3.h>

#include "Directive.h"
#include "Preference.h"
#include "TimeHelper.h"

struct TemperatureReadingEntity {
  int id = 0;
  int temperature = 0;
  int temperatureTargetStart = 0;
  int temperatureTargetStop = 0;
  int humidity = 0;
  long int time = 0;
};

struct TemperatureReadingsDto {
  int numRows = 0;
  TemperatureReadingEntity* temperatureReadings = nullptr;
};

struct CommandEntity {
  int id = 0;
  std::string command{};
  int temperature = 0;
  int temperature_target = 0;
  long int time = 0;
};

struct CommandsDto {
  int numRows = 0;
  CommandEntity* commands = nullptr;
};

struct PreferenceEntity {
  std::string acMode{};
  bool acEnabled = false;
  int acTemperatureStart = false;
  int acTemperatureStop = false;
  std::string irLastACCommand{};
  bool irLightToggled = false;
  int tdTemperatureTarget = 0;
};

class DatabaseHelper {
  TimeHelper &timeHelper;

  sqlite3* database;
  sqlite3_stmt* statement;
  std::string sql{};
  int responseCode;

 public:
  explicit DatabaseHelper(TimeHelper &timeHelper);

  bool setup();

  void insertTemperatureReading(
    int temperature, int temperatureTargetStart, int temperatureTargetStop, int humidity, bool force = false
  );

  TemperatureReadingsDto* selectTemperatureReadings(int hours);

  TemperatureReadingsDto* selectTemperatureReadings(int everyMinutes, int hours);

  void insertCommand(const char* command, int temperature, int temperature_target);

  CommandsDto* selectCommands(int maxRows = 5);

  PreferenceEntity* selectPreference();

  void updatePreferenceByType(Preference preference, const void* preferenceValue);

 private:
  int prepare();

#if APP_DEBUG
  void expandSql() const;
#endif
};
