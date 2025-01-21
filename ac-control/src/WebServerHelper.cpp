#include "Directive.h"
#include "WebServerHelper.h"

WebServerHelper::WebServerHelper(
  ACControl &acControl,
  TemperatureSensor &temperatureSensor,
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData,
  DatabaseHelper &databaseHelper,
  ACMode &acMode
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
  <p>
    A/C Mode: __AC_MODE__ (<a href="/change-mode">Change</a>)
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
    <a href="/temperature-history">Temperature history</a>
  </p>
  <br>

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
    stringReplace(html, "__AC_MODE__", ACModes[acMode]);

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

    auto CommandRows = std::string();
    const auto commands = databaseHelper.selectCommands(10);
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
      stringReplace(CommandRows, "__COMMAND_ROW_DATE_TIME__", TimeHelper::formatForHuman(command.time).c_str());
    }

    delete[] commands->commands;
    delete commands;

    stringReplace(html, "__COMMANDS_ROWS__", CommandRows.c_str());

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

    databaseHelper.updatePreferenceTdTemperatureTarget(temperatureData.temperatureTarget);

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

    databaseHelper.updatePreferenceTdTemperatureTarget(temperatureData.temperatureTarget);

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

  webServer.on("/change-mode", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /change-mode");
#endif

    if (acMode == Cold) {
      acMode = Heat;

      // reset temperature target to default
      temperatureData.temperatureTarget = temperatureData.temperatureTargetHeat;
    } else {
      acMode = Cold;

      // reset temperature target to default
      temperatureData.temperatureTarget = temperatureData.temperatureTargetCold;
    }

    databaseHelper.updatePreferenceAcMode(ACModes[acMode]);
    databaseHelper.updatePreferenceTdTemperatureTarget(temperatureData.temperatureTarget);

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });

  webServer.on("/temperature-history", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if APP_DEBUG
    Serial.println("GET /temperature-history");
#endif

    std::string json = "[";

    const auto temperatureReadings =
      databaseHelper.selectTemperatureReadings(6 * 60 / (APP_TEMPERATURE_SENSOR_DATABASE_DELAY / (60 * 1000)));
    for (auto i = 0; i < temperatureReadings->numRows; ++i) {
      const auto temperatureReading = temperatureReadings->temperatureReadings[i];

      json += "{\"temperature\": " + std::to_string(temperatureReading.temperature / 10.0) +
        ", \"humidity\": " + std::to_string(temperatureReading.humidity / 10.0) + ", \"time\": \"" +
        TimeHelper::formatForCode(temperatureReading.time) + "\"},";
    }

    if (temperatureReadings->numRows > 1) {
      json.pop_back();
    }

    delete[] temperatureReadings->temperatureReadings;
    delete temperatureReadings;

    json += "]";

    auto html = std::string(R"==(
<!DOCTYPE html>
<html lang="en">
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<link rel="shortcut icon" href="https://cdn-icons-png.flaticon.com/512/3274/3274588.png"/>
<style>
    body {
        text-align: center;
        background-color: black;
        color: white;
    }

    a {
        text-decoration: none;
        color: lightskyblue;
    }

    a:hover {
        color: #ffffff;
    }

    a:active {
        color: #78adff;
    }
</style>
<head>
    <title>Temperature chart</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@4"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns@3"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-annotation@3"></script>
</head>
<body onload="init()">
<h2>Temperature history (<a href="/">Back</a>)</h2>

<canvas id="chart"></canvas>

<script>
    function init() {
        let chart = new Chart(document.getElementById('chart'), {
            type: "line",
            options: {
                responsive: true,
                interaction: {
                    mode: 'index',
                    intersect: false,
                },
                stacked: false,
                scales: {
                    x: {
                        type: "time",
                        distribution: "linear",
                    },
                    yTemperature: {
                        min: 10,
                        max: 30,
                        type: 'linear',
                        display: true,
                        position: 'left',
                    },
                    yHumidity: {
                        min: 50,
                        max: 70,
                        type: 'linear',
                        display: true,
                        position: 'right',

                        // grid line settings
                        grid: {
                            drawOnChartArea: false, // only want the grid lines for one axis to show up
                        },
                    },
                },
                plugins: {
                    legend: {
                        display: true,
                    },
                    annotation: {
                        annotations: {
                            lineMin: {
                                type: 'line',
                                yMin: __TEMPERATURE_TARGET_START__,
                                yMax: __TEMPERATURE_TARGET_START__,
                                borderWidth: 2,
                                borderColor: '__TEMPERATURE_TARGET_START_COLOR__',
                                label: {
                                    display: true,
                                    content: '__TEMPERATURE_TARGET_START_LABEL__',
                                    position: 'start',
                                },
                            },
                            lineMax: {
                                type: 'line',
                                yMin: __TEMPERATURE_TARGET_STOP__,
                                yMax: __TEMPERATURE_TARGET_STOP__,
                                borderWidth: 2,
                                borderColor: '__TEMPERATURE_TARGET_STOP_COLOR__',
                                label: {
                                    display: true,
                                    content: '__TEMPERATURE_TARGET_STOP_LABEL__',
                                    position: 'start',
                                },
                            },
                        },
                    },
                },
            },
            data: {
                labels: [],
                datasets: [
                    {
                        data: [],
                        label: "Temperature",
                        borderColor: "#3e95cd",
                        fill: false,
                        yAxisID: 'yTemperature',
                    },
                    {
                        data: [],
                        label: "Humidity",
                        borderColor: "#bababa",
                        fill: false,
                        yAxisID: 'yHumidity',
                    },
                ],
            },
        });

        let json = __JSON_DATA__;

        json.reverse();

        for (json of json) {
            chart.data.datasets[0].data.push({
                x: json.time,
                y: json.temperature,
            });
            chart.data.datasets[1].data.push({
                x: json.time,
                y: json.humidity,
            });
        }

        chart.update();
    }
</script>
</body>
</html>
)==");

    stringReplace(
      html,
      "__TEMPERATURE_TARGET_START__",
      std::to_string(temperatureData.temperatureTargetStart() / 10.0).c_str()
    );
    stringReplace(
      html,
      "__TEMPERATURE_TARGET_STOP__",
      std::to_string(temperatureData.temperatureTargetStop() / 10.0).c_str()
    );
    if (acMode == Cold) {
      stringReplace(html, "__TEMPERATURE_TARGET_START_COLOR__", "green");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_COLOR__", "red");

      stringReplace(html, "__TEMPERATURE_TARGET_START_LABEL__", "Stop");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_LABEL__", "Start");
    } else {
      stringReplace(html, "__TEMPERATURE_TARGET_START_COLOR__", "red");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_COLOR__", "green");

      stringReplace(html, "__TEMPERATURE_TARGET_START_LABEL__", "Start");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_LABEL__", "Stop");
    }

    stringReplace(html, "__JSON_DATA__", json.c_str());

    webServer.send(200, "text/html", html.c_str());
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
  while (string.find(find) != std::string::npos) {
    string.replace(string.find(find), std::string(find).length(), replace);
  }
}
