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
  int acTemperatureStart = 0;
  int acTemperatureStop = 0;
  bool acTurnOffInsteadOfStop = false;
  std::string irLastACCommand{};
  bool irLightToggled = false;
  int tdTemperatureTarget = 0;
};

struct LogEntity {
  int id = 0;
  std::string log{};
  std::string filename{};
  int line = 0;
  long int time = 0;
};

struct LogsDto {
  int numRows = 0;
  LogEntity* logs = nullptr;
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

  TemperatureReadingsDto* selectTemperatureReadings(int startTime, int endTime);

  void insertCommand(const char* command, int temperature, int temperature_target);

  CommandsDto* selectCommands(int maxRows = 5);

  PreferenceEntity* selectPreference();

  void updatePreferenceByType(Preference preference, const void* preferenceValue);

  void insertLog(const char* filename, int line, const char* format, ...);

  LogsDto* selectLogs(int maxRows = 25);

 private:
  int prepare();

#if APP_DEBUG
  void expandSql() const;
#endif
};
