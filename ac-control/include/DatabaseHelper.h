#pragma once

#include <sqlite3.h>

#include "TimeHelper.h"

struct TemperatureReading {
  int id = 0;
  int temperature = 0;
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

class DatabaseHelper {
  TimeHelper &timeHelper;

  sqlite3* database;
  sqlite3_stmt* statement;
  int responseCode;
  char sql[255]{};

 public:
  explicit DatabaseHelper(TimeHelper &timeHelper);

  void setup();

  int prepare();

  void insertTemperatureReading(int temperature, int humidity);

  TemperatureReadings* selectTemperatureReadings(int maxRows = 5);

  void insertCommand(const char* command, int temperature, int temperature_target);

  Commands* selectCommands(int maxRows = 5);
};
