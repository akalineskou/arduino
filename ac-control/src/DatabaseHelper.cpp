#include <Arduino.h>

#include "DatabaseHelper.h"
#include "Directive.h"
#include "Helper.h"

DatabaseHelper::DatabaseHelper(TimeHelper &timeHelper): timeHelper(timeHelper) {
  database = nullptr;
  statement = nullptr;
}

bool DatabaseHelper::setup() {
  sqlite3_initialize();

  if (sqlite3_open("/sd/db.sqlite", &database) != SQLITE_OK) {
#if APP_DEBUG
    Serial.printf("SQL open error: %s\n", sqlite3_errmsg(database));
#endif

    return false;
  }

  sql = "PRAGMA page_size=512";
  prepare();
  sqlite3_step(statement);
  sqlite3_finalize(statement);

  sql = "PRAGMA cache_size=0";
  prepare();
  sqlite3_step(statement);
  sqlite3_finalize(statement);

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
ORDER BY time DESC
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

    if (sqlite3_step(statement) == SQLITE_ROW) {
      hasCount = sqlite3_column_int(statement, 0);
    } else {
#if APP_DEBUG && APP_DEBUG_DATABASE_TEMPERATURE_CHECK
      Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
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

  if (sqlite3_step(statement) == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

TemperatureReadingsDto* DatabaseHelper::selectTemperatureReadings(const int hours) {
#if APP_DEBUG
  Serial.println("Selecting temperature readings");
#endif

  sql = R"(
SELECT COUNT(id)
FROM temperature_readings
WHERE time >= ?1
ORDER BY time DESC
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, timeHelper.currentTime - hours * 3600);

#if APP_DEBUG
  expandSql();
#endif

  int numRows = 0;

  if (sqlite3_step(statement) == SQLITE_ROW) {
    numRows = sqlite3_column_int(statement, 0);
  } else {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
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
WHERE time >= ?1
ORDER BY time DESC
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, timeHelper.currentTime - hours * 3600);

#if APP_DEBUG
  expandSql();
#endif

  const auto temperatureReadings = new TemperatureReadingsDto{};
  temperatureReadings->temperatureReadings = new TemperatureReadingEntity[numRows];
  temperatureReadings->numRows = numRows;

  int i = 0;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    temperatureReadings->temperatureReadings[i].id = sqlite3_column_int(statement, 0);
    temperatureReadings->temperatureReadings[i].temperature = sqlite3_column_int(statement, 1);
    temperatureReadings->temperatureReadings[i].temperatureTargetStart = sqlite3_column_int(statement, 2);
    temperatureReadings->temperatureReadings[i].temperatureTargetStop = sqlite3_column_int(statement, 3);
    temperatureReadings->temperatureReadings[i].humidity = sqlite3_column_int(statement, 4);
    temperatureReadings->temperatureReadings[i].time = sqlite3_column_int(statement, 5);

    i++;
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  return temperatureReadings;
}

TemperatureReadingsDto* DatabaseHelper::selectTemperatureReadings(const int everyMinutes, const int hours) {
#if APP_DEBUG
  Serial.printf("Selecting temperature readings every %d minutes and %d hours\n", everyMinutes, hours);
#endif

  sql = R"(
  SELECT
      COUNT(time_rounded) AS total_row_count
  FROM (
           SELECT
               (time + ?1 / 2 * 60) / (?1 * 60) * (?1 * 60) AS time_rounded
           FROM
               temperature_readings
           WHERE
               time >= ?2
           GROUP BY
               time_rounded
       )
  )";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, everyMinutes);
  sqlite3_bind_int(statement, 2, timeHelper.currentTime - hours * 3600);

#if APP_DEBUG
  expandSql();
#endif

  int numRows = 0;

  if (sqlite3_step(statement) == SQLITE_ROW) {
    numRows = sqlite3_column_int(statement, 0);
  } else {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);

  if (numRows == 0) {
    return nullptr;
  }

  sql = R"(
SELECT
    id,
    round(avg(temperature)) AS temperature,
    temperatureTargetStart,
    temperatureTargetStop,
    round(avg(humidity)) AS humidity,
    (time + ?1 / 2 * 60) / (?1 * 60) * (?1 * 60) AS time_rounded
FROM
    temperature_readings
WHERE
    time >= ?2
GROUP BY
    time_rounded
ORDER BY
    time_rounded DESC;
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, everyMinutes);
  sqlite3_bind_int(statement, 2, timeHelper.currentTime - hours * 3600);

#if APP_DEBUG
  expandSql();
#endif

  const auto temperatureReadings = new TemperatureReadingsDto{};
  temperatureReadings->temperatureReadings = new TemperatureReadingEntity[numRows];
  temperatureReadings->numRows = numRows;

  int i = 0;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    temperatureReadings->temperatureReadings[i].id = sqlite3_column_int(statement, 0);
    temperatureReadings->temperatureReadings[i].temperature = sqlite3_column_int(statement, 1);
    temperatureReadings->temperatureReadings[i].temperatureTargetStart = sqlite3_column_int(statement, 2);
    temperatureReadings->temperatureReadings[i].temperatureTargetStop = sqlite3_column_int(statement, 3);
    temperatureReadings->temperatureReadings[i].humidity = sqlite3_column_int(statement, 4);
    temperatureReadings->temperatureReadings[i].time = sqlite3_column_int(statement, 5);

    i++;
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

  if (sqlite3_step(statement) == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
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
ORDER BY time DESC
LIMIT ?1
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

  sqlite3_bind_int(statement, 1, maxRows);

#if APP_DEBUG
  expandSql();
#endif

  const auto commands = new CommandsDto{};
  commands->commands = new CommandEntity[maxRows];

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

PreferenceEntity* DatabaseHelper::selectPreference() {
#if APP_DEBUG
  Serial.println("Selecting preference");
#endif

  sql = R"(
SELECT acMode, acEnabled, acTemperatureStart, acTemperatureStop, irLastACCommand, irLightToggled, tdTemperatureTarget
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

  if (sqlite3_step(statement) == SQLITE_ROW) {
    preference = new PreferenceEntity{};
    preference->acMode = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    preference->acEnabled = sqlite3_column_int(statement, 1);
    preference->acTemperatureStart = sqlite3_column_int(statement, 2);
    preference->acTemperatureStop = sqlite3_column_int(statement, 3);
    preference->irLastACCommand = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 4)));
    preference->irLightToggled = sqlite3_column_int(statement, 5);
    preference->tdTemperatureTarget = sqlite3_column_int(statement, 6);
  } else {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
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
      preference == IrLightToggled || preference == TdTemperatureTarget) {
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

  if (sqlite3_step(statement) == SQLITE_ERROR) {
#if APP_DEBUG
    Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
  }

  sqlite3_clear_bindings(statement);
  sqlite3_finalize(statement);
}

int DatabaseHelper::prepare() {
  const auto responseCode = sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, nullptr);
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
