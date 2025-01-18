#include <SdHelper.h>

#include "Directive.h"
#include "WebServerHelper.h"

WebServerHelper::WebServerHelper(
  ACControl &acControl,
  TemperatureSensor &temperatureSensor,
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData,
  DatabaseHelper &databaseHelper,
  const ACMode &acMode
):
    acControl(acControl),
    temperatureSensor(temperatureSensor),
    infraredTransmitter(infraredTransmitter),
    temperatureData(temperatureData),
    databaseHelper(databaseHelper),
    acMode(acMode),
    webServer(80) {}

void WebServerHelper::setup(const char* webServerAuthUsername, const char* webServerAuthPassword) {
  webServer.begin();

  webServer.on("/", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /");
#endif

    auto html = std::string(R"==(
<!DOCTYPE html>
<html lang="en">
<head>
  <title>A/C Control</title>
  <meta charset="utf-8">
  <!--<meta http-equiv="refresh" content="10">-->
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <link rel="shortcut icon" href="https://cdn-icons-png.flaticon.com/512/3274/3274588.png" />
  <style>
    body {
      text-align: center;
      margin-top: 8em;
      background-color: black;
      color: white;
    }

    a {
      text-decoration:none;
      color: lightskyblue;
    }
    a:hover {
      color: #ffffff;
    }
    a:active {
      color: #78adff;
    }
  </style>
</head>
<body>
  <p>
    A/C Control: __AC_CONTROL_STATUS__
  </p>
  <p>
    Last A/C Command: <b>__LAST_AC_COMMAND__</b> (<a href="/force-ac-start">Start</a> | <a href="/force-ac-stop">Stop</a>)
  </p>
  <br>

  <p>
    <a href="/decrease-target">__TARGET_TEMPERATURE_DECREASE__</a>
    &nbsp; | &nbsp; Target: <b>__TEMPERATURE_TARGET__</b>
    &nbsp; | &nbsp; <a href="/increase-target">__TARGET_TEMPERATURE_INCREASE__</a>
  </p>
  <br>

  <p>
    Temperature: <b>__TEMPERATURE__</b> (<small>__HUMIDITY__</small>)
  </p>
  <p>
    __TEMPERATURE_START_STOP__
  </p>
  <br>

  <p>
    <table>
      <tr>
        <th>Temperature</th>
        <th>Humidity</th>
        <th>Time</th>
      </tr>

    __TEMPERATURE_READING_ROWS__
    </table>
  </p>

  <p>
    <table>
      <tr>
        <th>Command</th>
        <th>Temperature</th>
        <th>Temperature Target</th>
        <th>Time</th>
      </tr>

    __COMMANDS_ROWS__
    </table>
  </p>
</body>
    )==");

    stringReplace(
      html,
      "__AC_CONTROL_STATUS__",
      acControl.isEnabled() ? R"==(<b>Enabled</b> (<a href="/disable">Disable</a>))=="
                            : R"==(<b>Disabled</b> (<a href="/enable">Enable</a>))=="
    );
    stringReplace(html, "__LAST_AC_COMMAND__", ACCommands[infraredTransmitter.lastACCommand]);

    stringReplace(
      html,
      "__TEMPERATURE__",
      TemperatureSensor::formatTemperature(temperatureSensor.getTemperature()).c_str()
    );
    stringReplace(html, "__HUMIDITY__", TemperatureSensor::formatHumidity(temperatureSensor.getHumidity()).c_str());

    stringReplace(
      html,
      "__TEMPERATURE_TARGET__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTarget).c_str()
    );
    if (acMode == Cold) {
      stringReplace(
        html,
        "__TEMPERATURE_START_STOP__",
        "Stop <small>&lt;</small> <b>__TEMPERATURE_STOP__</b> "
        "&nbsp; | "
        "&nbsp; Start <small>&gt;</small> "
        "<b>__TEMPERATURE_START__</b>"
      );
    } else {
      stringReplace(
        html,
        "__TEMPERATURE_START_STOP__",
        "Start <b><small>&lt;</small> __TEMPERATURE_START__</b> "
        "&nbsp; | "
        "&nbsp; Stop <b><small>&gt;</small> "
        "__TEMPERATURE_STOP__</b>"
      );
    }
    stringReplace(
      html,
      "__TEMPERATURE_START__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStart()).c_str()
    );
    stringReplace(
      html,
      "__TEMPERATURE_STOP__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStop()).c_str()
    );
    stringReplace(
      html,
      "__TARGET_TEMPERATURE_INCREASE__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTarget + static_cast<int>(0.5 * 10)).c_str()
    );
    stringReplace(
      html,
      "__TARGET_TEMPERATURE_DECREASE__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTarget - static_cast<int>(0.5 * 10)).c_str()
    );

    // todo add chart with more data
    auto TemperatureReadingRows = std::string();
    const auto temperatureReadings = databaseHelper.selectTemperatureReadings();
    for (auto i = 0; i < temperatureReadings->numRows; ++i) {
      const auto temperatureReading = temperatureReadings->temperatureReadings[i];

      TemperatureReadingRows += std::string(R"==(
<tr>
  <td>__TEMPERATURE_READING_ROW_TEMPERATURE__</td>
  <td>__TEMPERATURE_READING_ROW_HUMIDITY__</td>
  <td>__TEMPERATURE_READING_ROW_DATE_TIME__</td>
</tr>
)==");

      stringReplace(
        TemperatureReadingRows,
        "__TEMPERATURE_READING_ROW_TEMPERATURE__",
        TemperatureSensor::formatTemperature(temperatureReading.temperature).c_str()
      );
      stringReplace(
        TemperatureReadingRows,
        "__TEMPERATURE_READING_ROW_HUMIDITY__",
        TemperatureSensor::formatHumidity(temperatureReading.humidity).c_str()
      );
      stringReplace(
        TemperatureReadingRows,
        "__TEMPERATURE_READING_ROW_DATE_TIME__",
        TimeHelper::getDateTimeFormatted(temperatureReading.time).c_str()
      );
    }

    stringReplace(html, "__TEMPERATURE_READING_ROWS__", TemperatureReadingRows.c_str());

    delete[] temperatureReadings->temperatureReadings;
    delete temperatureReadings;

    auto CommandRows = std::string();
    const auto commands = databaseHelper.selectCommands();
    for (auto i = 0; i < commands->numRows; ++i) {
      const auto command = commands->commands[i];

      CommandRows += std::string(R"==(
<tr>
  <td>__COMMAND_ROW_COMMAND__</td>
  <td>__COMMAND_ROW_TEMPERATURE__</td>
  <td>__COMMAND_ROW_TEMPERATURE_TARGET__</td>
  <td>__COMMAND_ROW_DATE_TIME__</td>
</tr>
)==");

      stringReplace(CommandRows, "__COMMAND_ROW_COMMAND__", command.command.c_str());
      stringReplace(
        CommandRows,
        "__COMMAND_ROW_TEMPERATURE__",
        TemperatureSensor::formatTemperature(command.temperature).c_str()
      );
      stringReplace(
        CommandRows,
        "__COMMAND_ROW_TEMPERATURE_TARGET__",
        TemperatureSensor::formatTemperature(command.temperature_target).c_str()
      );
      stringReplace(CommandRows, "__COMMAND_ROW_DATE_TIME__", TimeHelper::getDateTimeFormatted(command.time).c_str());
    }

    stringReplace(html, "__COMMANDS_ROWS__", CommandRows.c_str());

    delete[] commands->commands;
    delete commands;

    webServer.send(200, "text/html", html.c_str());
  });

  webServer.on("/enable", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /enable");
#endif

    acControl.enable();

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/disable", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /disable");
#endif

    acControl.disable();

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });

  webServer.on("/increase-target", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /increase-target");
#endif

    temperatureData.temperatureTarget += 0.5 * 10;

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/decrease-target", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /decrease-target");
#endif

    temperatureData.temperatureTarget -= 0.5 * 10;

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/force-ac-start", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /force-ac-start");
#endif

    acControl.start();

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/force-ac-stop", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /force-ac-stop");
#endif

    acControl.stop();

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });

  webServer.onNotFound([this] {
    webServer.send(404, "text/html", R"==(
<!DOCTYPE html>
<html lang="en">
<head>
  <title>Resource not found</title>
  <meta charset="utf-8">
  <link rel="shortcut icon" href="https://cdn-icons-png.flaticon.com/512/3274/3274588.png" />
</head>
<body style="text-align: center;">
  <p>The resource was not found.</p>
  <p><a href="/">Start again</a></p>
</body>
    )==");
  });
}

void WebServerHelper::loop() {
  webServer.handleClient();
}

bool WebServerHelper::isAuthenticated(const char* webServerAuthUsername, const char* webServerAuthPassword) {
  if (strlen(webServerAuthUsername) == 0 && strlen(webServerAuthPassword) == 0) {
    // authentication disabled
    return true;
  }

  return webServer.authenticate(webServerAuthUsername, webServerAuthPassword);
}

void WebServerHelper::stringReplace(std::string &string, const char* find, const char* replace) {
  string.replace(string.find(find), std::string(find).length(), replace);
}
