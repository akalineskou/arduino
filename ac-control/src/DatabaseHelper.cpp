#include <Arduino.h>

#include "DatabaseHelper.h"
#include "Directive.h"

DatabaseHelper::DatabaseHelper(TimeHelper &timeHelper): timeHelper(timeHelper) {
  database = nullptr;
  statement = nullptr;
  responseCode = 0;
}

void DatabaseHelper::setup() {
  sqlite3_initialize();

  responseCode = sqlite3_open("/sd/db.sqlite", &database);
  if (responseCode != SQLITE_OK) {
#if APP_DEBUG
    Serial.printf("SQL open error: %s\n", sqlite3_errmsg(database));
#endif

    while (true);
  }

  sprintf(sql, "PRAGMA page_size=512;");
  prepare();
  sqlite3_step(statement);
  sqlite3_finalize(statement);

  sprintf(sql, "PRAGMA cache_size=0;");
  prepare();
  sqlite3_step(statement);
  sqlite3_finalize(statement);
}

int DatabaseHelper::prepare() {
  responseCode = sqlite3_prepare_v2(database, sql, -1, &statement, nullptr);
  if (responseCode != SQLITE_OK) {
#if APP_DEBUG
    Serial.printf("SQL prepare error: %s\n", sqlite3_errmsg(database));
#endif

    sqlite3_finalize(statement);
  }

#if APP_DEBUG
  const auto expandedSql = sqlite3_expanded_sql(statement);
  Serial.println(expandedSql);
  sqlite3_free(expandedSql);
#endif

  return responseCode;
}

void DatabaseHelper::insertTemperatureReading(const int temperature, const int humidity) {
#if APP_DEBUG
  Serial.println("Inserting temperature reading");
#endif

  sprintf(sql, "INSERT INTO temperature_readings (id, temperature, humidity, time) VALUES(null, ?1, ?2, ?3)");

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, temperature);
  sqlite3_bind_int(statement, 2, humidity);
  sqlite3_bind_int(statement, 3, timeHelper.currentTime);

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

TemperatureReadings* DatabaseHelper::selectTemperatureReadings(const int maxRows) {
#if APP_DEBUG
  Serial.println("Selecting temperature readings");
#endif

  sprintf(sql, "SELECT id, temperature, humidity, time FROM temperature_readings ORDER BY time DESC LIMIT ?1");

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, maxRows);

  const auto temperatureReadings = new TemperatureReadings{};
  temperatureReadings->temperatureReadings = new TemperatureReading[maxRows];

  int i = 0;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    temperatureReadings->temperatureReadings[i].id = sqlite3_column_int(statement, 0);
    temperatureReadings->temperatureReadings[i].temperature = sqlite3_column_int(statement, 1);
    temperatureReadings->temperatureReadings[i].humidity = sqlite3_column_int(statement, 2);
    temperatureReadings->temperatureReadings[i].time = sqlite3_column_int(statement, 3);

    i++;
  }

  temperatureReadings->numRows = i;

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return temperatureReadings;
}

void DatabaseHelper::insertCommand(const char* command, const int temperature, const int temperature_target) {
#if APP_DEBUG
  Serial.println("Inserting command");
#endif

  sprintf(
    sql,
    "INSERT INTO commands (id, command, temperature, temperature_target, time) VALUES (null, ?1, ?2, ?3, ?4)"
  );

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_text(statement, 1, command, static_cast<int>(strlen(command)), SQLITE_STATIC);
  sqlite3_bind_int(statement, 2, temperature);
  sqlite3_bind_int(statement, 3, temperature_target);
  sqlite3_bind_int(statement, 4, timeHelper.currentTime);

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

Commands* DatabaseHelper::selectCommands(const int maxRows) {
#if APP_DEBUG
  Serial.println("Selecting commands");
#endif

  sprintf(sql, "SELECT id, command, temperature, temperature_target, time FROM commands ORDER BY time DESC LIMIT ?1");

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, maxRows);

  const auto commands = new Commands{};
  commands->commands = new Command[maxRows];

  int i = 0;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    commands->commands[i].id = sqlite3_column_int(statement, 0);
    commands->commands[i].command = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
    commands->commands[i].temperature = sqlite3_column_int(statement, 2);
    commands->commands[i].temperature_target = sqlite3_column_int(statement, 3);
    commands->commands[i].time = sqlite3_column_int(statement, 4);

    i++;
  }

  commands->numRows = i;

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return commands;
}
