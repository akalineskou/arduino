#include <Arduino.h>

#include "DatabaseHelper.h"
#include "Directive.h"
#include "Helper.h"

DatabaseHelper::DatabaseHelper(TimeHelper &timeHelper): timeHelper(timeHelper) {
  database = nullptr;
  statement = nullptr;
  responseCode = 0;
}

bool DatabaseHelper::setup() {
  sqlite3_initialize();

  if ((responseCode = sqlite3_open("/sd/db.sqlite", &database)) != SQLITE_OK) {
#if APP_DEBUG
    Serial.printf("SQL open error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif

    return false;
  }

  const auto pragmas = {"PRAGMA page_size=512", "PRAGMA cache_size=0"};
  for (const auto pragma: pragmas) {
    sql = pragma;
    if (prepare() == SQLITE_OK) {
#if APP_DEBUG
      expandSql();
#endif

      if ((responseCode = sqlite3_step(statement)) != SQLITE_DONE) {
#if APP_DEBUG
        Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
      }

      sqlite3_finalize(statement);
    }
  }

  return true;
}

void DatabaseHelper::insertTemperatureReading(
  const int temperature,
  const int temperatureTargetStart,
  const int temperatureTargetStop,
  const int humidity,
  const bool force
) {
  if (!force) {
    // check more than 5 minutes passed
    sql = R"(
SELECT time + ?1 > ?2
FROM temperature_readings
ORDER BY time DESC, id DESC
LIMIT 1
)";

    if (prepare() != SQLITE_OK) {
      return;
    }

    sqlite3_bind_int(statement, 1, 5 * 60);
    sqlite3_bind_int(statement, 2, timeHelper.currentTime);

#if APP_DEBUG && APP_DEBUG_DATABASE_TEMPERATURE_CHECK
    expandSql();
#endif

    bool hasCount = false;

    if ((responseCode = sqlite3_step(statement)) == SQLITE_ROW) {
      hasCount = sqlite3_column_int(statement, 0);
    } else {
#if APP_DEBUG && APP_DEBUG_DATABASE_TEMPERATURE_CHECK
      Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
    }

    sqlite3_clear_bindings(statement);
    sqlite3_finalize(statement);

    if (hasCount) {
      // a temperature reading already exists in the 5-minute time span
#if APP_DEBUG && APP_DEBUG_DATABASE_TEMPERATURE_CHECK
      Serial.println("Skipping temperature reading insert");
#endif
      return;
    }
  }

#if APP_DEBUG
  Serial.println("Inserting temperature reading");
#endif

  sql = R"(
INSERT INTO temperature_readings (id, temperature, temperatureTargetStart, temperatureTargetStop, humidity, time)
VALUES(null, ?1, ?2, ?3, ?4, ?5)
)";

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, temperature);
  sqlite3_bind_int(statement, 2, temperatureTargetStart);
  sqlite3_bind_int(statement, 3, temperatureTargetStop);
  sqlite3_bind_int(statement, 4, humidity);
  sqlite3_bind_int(statement, 5, timeHelper.currentTime);

#if APP_DEBUG
  expandSql();
#endif

  if ((responseCode = sqlite3_step(statement)) == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

TemperatureReadingsDto* DatabaseHelper::selectTemperatureReadings(const int startTime, const int endTime) {
#if APP_DEBUG
  Serial.println("Selecting temperature readings");
#endif

  sql = R"(
SELECT COUNT(id)
FROM temperature_readings
WHERE time BETWEEN ?1 AND ?2
ORDER BY time DESC, id DESC
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, startTime);
  sqlite3_bind_int(statement, 2, endTime);

#if APP_DEBUG
  expandSql();
#endif

  int numRows = 0;

  if ((responseCode = sqlite3_step(statement)) == SQLITE_ROW) {
    numRows = sqlite3_column_int(statement, 0);
  } else {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  if (numRows == 0) {
    return nullptr;
  }

  sql = R"(
SELECT id, temperature, temperatureTargetStart, temperatureTargetStop, humidity, time
FROM temperature_readings
WHERE time BETWEEN ?1 AND ?2
ORDER BY time DESC, id DESC
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, startTime);
  sqlite3_bind_int(statement, 2, endTime);

#if APP_DEBUG
  expandSql();
#endif

  auto temperatureReadings = new TemperatureReadingsDto{};
  temperatureReadings->temperatureReadings = new TemperatureReadingEntity[numRows];
  temperatureReadings->numRows = numRows;

  int i = 0;
  while ((responseCode = sqlite3_step(statement)) == SQLITE_ROW) {
    temperatureReadings->temperatureReadings[i].id = sqlite3_column_int(statement, 0);
    temperatureReadings->temperatureReadings[i].temperature = sqlite3_column_int(statement, 1);
    temperatureReadings->temperatureReadings[i].temperatureTargetStart = sqlite3_column_int(statement, 2);
    temperatureReadings->temperatureReadings[i].temperatureTargetStop = sqlite3_column_int(statement, 3);
    temperatureReadings->temperatureReadings[i].humidity = sqlite3_column_int(statement, 4);
    temperatureReadings->temperatureReadings[i].time = sqlite3_column_int(statement, 5);

    i++;
  }

  if (responseCode != SQLITE_DONE) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  if (i == 0) {
    delete[] temperatureReadings->temperatureReadings;
    delete temperatureReadings;

    temperatureReadings = nullptr;
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return temperatureReadings;
}

void DatabaseHelper::insertCommand(const char* command, const int temperature, const int temperature_target) {
#if APP_DEBUG
  Serial.println("Inserting command");
#endif

  sql = R"(
INSERT INTO commands (id, command, temperature, temperature_target, time)
VALUES (null, ?1, ?2, ?3, ?4)
)";

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

  if ((responseCode = sqlite3_step(statement)) == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

CommandsDto* DatabaseHelper::selectCommands(const int maxRows) {
#if APP_DEBUG
  Serial.println("Selecting commands");
#endif

  sql = R"(
SELECT id, command, temperature, temperature_target, time
FROM commands
ORDER BY time DESC, id DESC
LIMIT ?1
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, maxRows);

#if APP_DEBUG
  expandSql();
#endif

  auto commands = new CommandsDto{};
  commands->commands = new CommandEntity[maxRows];

  int i = 0;
  while ((responseCode = sqlite3_step(statement)) == SQLITE_ROW) {
    commands->commands[i].id = sqlite3_column_int(statement, 0);
    commands->commands[i].command = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
    commands->commands[i].temperature = sqlite3_column_int(statement, 2);
    commands->commands[i].temperature_target = sqlite3_column_int(statement, 3);
    commands->commands[i].time = sqlite3_column_int(statement, 4);

    i++;
  }

  if (responseCode != SQLITE_DONE) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  if (i > 0) {
    commands->numRows = i;
  } else {
    delete[] commands->commands;
    delete commands;

    commands = nullptr;
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return commands;
}

PreferenceEntity* DatabaseHelper::selectPreference() {
#if APP_DEBUG
  Serial.println("Selecting preference");
#endif

  sql = R"(
SELECT acMode, acEnabled, acTemperatureStart, acTemperatureStop, acTurnOffInsteadOfStop, irLastACCommand, irLightToggled, tdTemperatureTarget
FROM preference
LIMIT 1
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

#if APP_DEBUG
  expandSql();
#endif

  PreferenceEntity* preference = nullptr;

  if ((responseCode = sqlite3_step(statement)) == SQLITE_ROW) {
    preference = new PreferenceEntity{};
    preference->acMode = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    preference->acEnabled = sqlite3_column_int(statement, 1);
    preference->acTemperatureStart = sqlite3_column_int(statement, 2);
    preference->acTemperatureStop = sqlite3_column_int(statement, 3);
    preference->acTurnOffInsteadOfStop = sqlite3_column_int(statement, 4);
    preference->irLastACCommand = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 5)));
    preference->irLightToggled = sqlite3_column_int(statement, 6);
    preference->tdTemperatureTarget = sqlite3_column_int(statement, 7);
  } else {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return preference;
}

void DatabaseHelper::updatePreferenceByType(const Preference preference, const void* preferenceValue) {
#if APP_DEBUG
  Serial.printf("Updating preference: %s\n", xPreferences[preference]);
#endif

  sql = R"(
UPDATE preference SET
__PREFERENCE__ = ?1
)";

  stringReplace(sql, "__PREFERENCE__", xPreferences[preference]);

  if (prepare() != SQLITE_OK) {
    return;
  }

  if (preference == AcEnabled || preference == AcTemperatureStart || preference == AcTemperatureStop ||
      preference == IrLightToggled || preference == AcTurnOffInsteadOfStop || preference == TdTemperatureTarget) {
    sqlite3_bind_int(statement, 1, reinterpret_cast<int>(preferenceValue));
  } else if (preference == AcMode || preference == IrLastACCommand) {
    sqlite3_bind_text(
      statement,
      1,
      static_cast<const char*>(preferenceValue),
      static_cast<int>(strlen(static_cast<const char*>(preferenceValue))),
      SQLITE_STATIC
    );
  }

#if APP_DEBUG
  expandSql();
#endif

  if ((responseCode = sqlite3_step(statement)) == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

int DatabaseHelper::prepare() {
  responseCode = sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr);
  if (responseCode != SQLITE_OK) {
#if APP_DEBUG
    Serial.printf("SQL prepare error: %s (%d)\n", sqlite3_errmsg(database), responseCode);
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
