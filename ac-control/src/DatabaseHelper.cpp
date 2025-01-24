#include <Arduino.h>

#include "DatabaseHelper.h"
#include "Directive.h"

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

  sql = "cache_size=0";
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
#if APP_DEBUG
  Serial.println("Inserting temperature reading");
#endif

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

#if APP_DEBUG
    expandSql();
#endif

    bool hasCount = false;

    if (sqlite3_step(statement) == SQLITE_ROW) {
      hasCount = sqlite3_column_int(statement, 0);
    } else {
#if APP_DEBUG
      Serial.printf("SQL step error: %s\n", sqlite3_errmsg(database));
#endif
    }

    sqlite3_clear_bindings(statement);
    sqlite3_finalize(statement);

    if (hasCount) {
      // a temperature reading already exists in the 5-minute time span
#if APP_DEBUG
      Serial.println("Skipping temperature reading insert");
#endif
      return;
    }
  }

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

TemperatureReadings* DatabaseHelper::selectTemperatureReadings(const int hours) {
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

  const auto temperatureReadings = new TemperatureReadings{};
  temperatureReadings->temperatureReadings = new TemperatureReading[numRows];
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

TemperatureReadings* DatabaseHelper::selectTemperatureReadings(const int everyMinutes, const int hours) {
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

  const auto temperatureReadings = new TemperatureReadings{};
  temperatureReadings->temperatureReadings = new TemperatureReading[numRows];
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

Commands* DatabaseHelper::selectCommands(const int maxRows) {
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

  sql = R"(
SELECT acEnabled, acMode, irLastACCommand, irLightToggled, tdTemperatureTarget
FROM preference
LIMIT 1
)";

  if (prepare() != SQLITE_OK) {
    return nullptr;
  }

#if APP_DEBUG
  expandSql();
#endif

  Preference* preference = nullptr;

  if (sqlite3_step(statement) == SQLITE_ROW) {
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

  sql = R"(
UPDATE preference SET
acEnabled = ?1
)";

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, acEnabled);

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

void DatabaseHelper::updatePreferenceAcMode(const char* acMode) {
#if APP_DEBUG
  Serial.println("Updating preference acMode");
#endif

  sql = R"(
UPDATE preference SET
acMode = ?1
)";

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_text(statement, 1, acMode, static_cast<int>(strlen(acMode)), SQLITE_STATIC);

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

void DatabaseHelper::updatePreferenceIrLastACCommand(const char* irLastACCommand) {
#if APP_DEBUG
  Serial.println("Updating preference irLastACCommand");
#endif

  sql = R"(
UPDATE preference SET
irLastACCommand = ?1
)";

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_text(statement, 1, irLastACCommand, static_cast<int>(strlen(irLastACCommand)), SQLITE_STATIC);

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

void DatabaseHelper::updatePreferenceIrLightToggled(const bool irLightToggled) {
#if APP_DEBUG
  Serial.println("Updating preference irLightToggled");
#endif

  sql = R"(
UPDATE preference SET
irLightToggled = ?1
)";

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, irLightToggled);

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

void DatabaseHelper::updatePreferenceTdTemperatureTarget(const int tdTemperatureTarget) {
#if APP_DEBUG
  Serial.println("Updating preference tdTemperatureTarget");
#endif

  sql = R"(
UPDATE preference SET
tdTemperatureTarget = ?1
)";

  if (prepare() != SQLITE_OK) {
    return;
  }

  sqlite3_bind_int(statement, 1, tdTemperatureTarget);

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
