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

#if APP_DEBUG
  expandSql();
#endif

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

#if APP_DEBUG
  expandSql();
#endif

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

#if APP_DEBUG
  expandSql();
#endif

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

#if APP_DEBUG
  expandSql();
#endif

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

Preference* DatabaseHelper::selectPreference() {
#if APP_DEBUG
  Serial.println("Selecting preference");
#endif

  sprintf(
    sql,
    "SELECT acEnabled, acMode, irLastACCommand, irLightToggled, tdTemperatureTarget FROM preference LIMIT 1"
  );

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

#if APP_DEBUG
  expandSql();
#endif

  Preference* preference = nullptr;

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ROW) {
    preference = new Preference{};
    preference->acEnabled = sqlite3_column_int(statement, 0);
    preference->acMode = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
    preference->irLastACCommand = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
    preference->irLightToggled = sqlite3_column_int(statement, 3);
    preference->tdTemperatureTarget = sqlite3_column_int(statement, 4);
  } else {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return preference;
}

void DatabaseHelper::updatePreferenceAcEnabled(const bool acEnabled) {
#if APP_DEBUG
  Serial.println("Updating preference acEnabled");
#endif

  sprintf(sql, "UPDATE preference SET acEnabled = ?1");

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, acEnabled);

#if APP_DEBUG
  expandSql();
#endif

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

void DatabaseHelper::updatePreferenceAcMode(const char* acMode) {
#if APP_DEBUG
  Serial.println("Updating preference acMode");
#endif

  sprintf(sql, "UPDATE preference SET acMode = ?1");

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_text(statement, 1, acMode, static_cast<int>(strlen(acMode)), SQLITE_STATIC);

#if APP_DEBUG
  expandSql();
#endif

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

void DatabaseHelper::updatePreferenceIrLastACCommand(const char* irLastACCommand) {
#if APP_DEBUG
  Serial.println("Updating preference irLastACCommand");
#endif

  sprintf(sql, "UPDATE preference SET irLastACCommand = ?1");

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_text(statement, 1, irLastACCommand, static_cast<int>(strlen(irLastACCommand)), SQLITE_STATIC);

#if APP_DEBUG
  expandSql();
#endif

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

void DatabaseHelper::updatePreferenceIrLightToggled(bool irLightToggled) {
#if APP_DEBUG
  Serial.println("Updating preference irLightToggled");
#endif

  sprintf(sql, "UPDATE preference SET irLightToggled = ?1");

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, irLightToggled);

#if APP_DEBUG
  expandSql();
#endif

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

void DatabaseHelper::updatePreferenceTdTemperatureTarget(int tdTemperatureTarget) {
#if APP_DEBUG
  Serial.println("Updating preference tdTemperatureTarget");
#endif

  sprintf(sql, "UPDATE preference SET tdTemperatureTarget = ?1");

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, tdTemperatureTarget);

#if APP_DEBUG
  expandSql();
#endif

  responseCode = sqlite3_step(statement);
  if (responseCode == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
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

  return responseCode;
}

#if APP_DEBUG
void DatabaseHelper::expandSql() const {
  const auto expandedSql = sqlite3_expanded_sql(statement);
  Serial.println(expandedSql);
  sqlite3_free(expandedSql);
}
#endif
