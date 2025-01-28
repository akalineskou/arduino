#include "Directive.h"
#include "Helper.h"
#include "Preference.h"
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
    <table style="width: 100%;">
      <tr>
        <th>Command</th>
        <th>Temperature</th>
        <th>Temperature Target</th>
        <th>Time</th>
      </tr>

    __COMMANDS_ROWS__
    </table>
  </p>

  <p>
    <table style="width: 100%;">
      <tr>
        <th>AcMode</th>
        <th>AcEnabled</th>
        <th>AcTemperatureStart</th>
        <th>AcTemperatureStop</th>
        <th>IrLastACCommand</th>
        <th>IrLightToggled</th>
        <th>TdTemperatureTarget</th>
      </tr>

      <tr>
        <td>__PREFERENCE_AcMode__</td>
        <td>__PREFERENCE_AcEnabled__</td>
        <td>__PREFERENCE_AcTemperatureStart__</td>
        <td>__PREFERENCE_AcTemperatureStop__</td>
        <td>__PREFERENCE_IrLastACCommand__</td>
        <td>__PREFERENCE_IrLightToggled__</td>
        <td>__PREFERENCE_TdTemperatureTarget__</td>
      </tr>
    </table>
  </p>

  <p>
    <table style="width: 100%;">
      <tr>
        <th>Heap size</th>
        <th>Free Heap</th>
        <th>Min Free Heap</th>
        <th>Max Alloc Heap</th>
      </tr>

      <tr>
        <td>__DEBUG_HEAP_SIZE__</td>
        <td>__DEBUG_FREE_HEAP__</td>
        <td>__DEBUG_MIN_FREE_HEAP__</td>
        <td>__DEBUG_MAX_ALLOC_HEAP__</td>
      </tr>
    </table>
  </p>
</body>
    )==");

    stringReplace(
      html,
      "__AC_CONTROL_STATUS__",
      acControl.enabled ? R"==(<b>Enabled</b> (<a href="/disable">Disable</a>))=="
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
        "Stop <small>&le;</small> <b>__TEMPERATURE_STOP__</b> "
        "&nbsp; | "
        "&nbsp; Start <small>&ge;</small> "
        "<b>__TEMPERATURE_START__</b>"
      );
    } else {
      stringReplace(
        html,
        "__TEMPERATURE_START_STOP__",
        "Start <b><small>&le;</small> __TEMPERATURE_START__</b> "
        "&nbsp; | "
        "&nbsp; Stop <b><small>&ge;</small> "
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

    auto commandRows = std::string();

    const auto commands = databaseHelper.selectCommands(10);
    if (commands != nullptr) {
      for (auto i = 0; i < commands->numRows; ++i) {
        const auto command = commands->commands[i];

        commandRows += std::string(R"==(
<tr>
  <td>__COMMAND_ROW_COMMAND__</td>
  <td>__COMMAND_ROW_TEMPERATURE__</td>
  <td>__COMMAND_ROW_TEMPERATURE_TARGET__</td>
  <td>__COMMAND_ROW_DATE_TIME__</td>
</tr>
)==");

        stringReplace(commandRows, "__COMMAND_ROW_COMMAND__", command.command.c_str());
        stringReplace(
          commandRows,
          "__COMMAND_ROW_TEMPERATURE__",
          TemperatureSensor::formatTemperature(command.temperature).c_str()
        );
        stringReplace(
          commandRows,
          "__COMMAND_ROW_TEMPERATURE_TARGET__",
          TemperatureSensor::formatTemperature(command.temperature_target).c_str()
        );
        stringReplace(commandRows, "__COMMAND_ROW_DATE_TIME__", TimeHelper::formatForHuman(command.time).c_str());
      }

      delete[] commands->commands;
      delete commands;
    }

    stringReplace(html, "__COMMANDS_ROWS__", commandRows.c_str());

    const auto preference = databaseHelper.selectPreference();

    stringReplace(html, "__PREFERENCE_AcMode__", preference->acMode.c_str());
    stringReplace(html, "__PREFERENCE_AcEnabled__", preference->acEnabled ? "Yes" : "No");
    stringReplace(html, "__PREFERENCE_AcTemperatureStart__", std::to_string(preference->acTemperatureStart).c_str());
    stringReplace(html, "__PREFERENCE_AcTemperatureStop__", std::to_string(preference->acTemperatureStop).c_str());
    stringReplace(html, "__PREFERENCE_IrLastACCommand__", preference->irLastACCommand.c_str());
    stringReplace(html, "__PREFERENCE_IrLightToggled__", preference->irLightToggled ? "Yes" : "No");
    stringReplace(html, "__PREFERENCE_TdTemperatureTarget__", std::to_string(preference->tdTemperatureTarget).c_str());

    delete preference;

    stringReplace(html, "__PREFERENCE__", html.c_str());

    stringReplace(html, "__DEBUG_HEAP_SIZE__", std::to_string(ESP.getHeapSize()).c_str());
    stringReplace(html, "__DEBUG_FREE_HEAP__", std::to_string(heap_caps_get_free_size(MALLOC_CAP_8BIT)).c_str());
    stringReplace(
      html,
      "__DEBUG_MIN_FREE_HEAP__",
      std::to_string(heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT)).c_str()
    );
    stringReplace(
      html,
      "__DEBUG_MAX_ALLOC_HEAP__",
      std::to_string(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)).c_str()
    );

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

    databaseHelper.updatePreferenceByType(TdTemperatureTarget, (void*) temperatureData.temperatureTarget);

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

    databaseHelper.updatePreferenceByType(TdTemperatureTarget, (void*) temperatureData.temperatureTarget);

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

    acControl.temperatureStart = -1;
    acControl.temperatureStop = -1;

    databaseHelper.updatePreferenceByType(AcMode, ACModes[acMode]);
    databaseHelper.updatePreferenceByType(AcTemperatureStart, (void*) acControl.temperatureStart);
    databaseHelper.updatePreferenceByType(AcTemperatureStop, (void*) acControl.temperatureStop);
    databaseHelper.updatePreferenceByType(TdTemperatureTarget, (void*) temperatureData.temperatureTarget);

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

    TemperatureReadingsDto* temperatureReadings;
    if (webServer.hasArg("everyMinutes") && webServer.hasArg("hours")) {
      temperatureReadings =
        databaseHelper.selectTemperatureReadings(webServer.arg("everyMinutes").toInt(), webServer.arg("hours").toInt());
    } else {
      temperatureReadings = databaseHelper.selectTemperatureReadings(6);
    }

    std::string json = "[";

    int temperatureMin = 9999;
    int temperatureMax = 0;
    int humidityMin = temperatureMin;
    int humidityMax = temperatureMax;

    if (temperatureReadings != nullptr) {
      for (auto i = 0; i < temperatureReadings->numRows; ++i) {
        const auto temperatureReading = temperatureReadings->temperatureReadings[i];

        json += R"=({"temperature":)=" + std::to_string(temperatureReading.temperature / 10.0) +
          R"=(,"temperatureTargetStart":)=" + std::to_string(temperatureReading.temperatureTargetStart / 10.0) +
          R"=(,"temperatureTargetStop":)=" + std::to_string(temperatureReading.temperatureTargetStop / 10.0) +
          R"=(,"humidity":)=" + std::to_string(temperatureReading.humidity / 10.0) +
          R"=(,"time":")=" + TimeHelper::formatForCode(temperatureReading.time) + R"=("},)=";

        if (temperatureReading.temperatureTargetStart < temperatureMin) {
          temperatureMin = temperatureReading.temperatureTargetStart;
        }
        if (temperatureReading.temperatureTargetStop > temperatureMax) {
          temperatureMax = temperatureReading.temperatureTargetStop;
        }

        if (temperatureReading.temperature < temperatureMin) {
          temperatureMin = temperatureReading.temperature;
        }
        if (temperatureReading.temperature > temperatureMax) {
          temperatureMax = temperatureReading.temperature;
        }

        if (temperatureReading.humidity < humidityMin) {
          humidityMin = temperatureReading.humidity;
        }
        if (temperatureReading.humidity > humidityMax) {
          humidityMax = temperatureReading.humidity;
        }
      }

      json.pop_back();

      delete[] temperatureReadings->temperatureReadings;
      delete temperatureReadings;
    }

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
</head>
<body onload="init()">
<h2>Temperature history <small>(6 hours) (<a href="/">Back</a>)</small></h2>

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
                        grid: {
                            color: "rgba(255, 255, 255, 0.2)",
                        },
                    },
                    yTemperature: {
                        min: __TEMPERATURE_MIN__,
                        max: __TEMPERATURE_MAX__,
                        type: 'linear',
                        display: true,
                        position: 'left',
                        grid: {
                            color: "rgba(255, 255, 255, 0.2)",
                        },
                        title: {
                            text: "Temperature",
                            display: true,
                            color: "#3e95cd",
                        },
                    },
                    yTemperatureTargetStart: {
                        min: __TEMPERATURE_MIN__,
                        max: __TEMPERATURE_MAX__,
                        type: 'linear',
                        display: false,
                    },
                    yTemperatureTargetStop: {
                        min: __TEMPERATURE_MIN__,
                        max: __TEMPERATURE_MAX__,
                        type: 'linear',
                        display: false,
                    },
                    yHumidity: {
                        min: __HUMIDITY_MIN__,
                        max: __HUMIDITY_MAX__,
                        type: 'linear',
                        display: true,
                        position: 'right',
                        title: {
                            text: "Humidity",
                            display: true,
                            color: "#bababa",
                        },
                        grid: {
                            drawOnChartArea: false,
                        },
                    },
                },
                plugins: {
                    legend: {
                        display: true,
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
                        label: "Temperature target __TEMPERATURE_TARGET_START_LABEL__",
                        borderColor: "__TEMPERATURE_TARGET_START_COLOR__",
                        fill: false,
                        yAxisID: 'yTemperatureTargetStart',
                    },
                    {
                        data: [],
                        label: "Temperature target __TEMPERATURE_TARGET_STOP_LABEL__",
                        borderColor: "__TEMPERATURE_TARGET_STOP_COLOR__",
                        fill: false,
                        yAxisID: 'yTemperatureTargetStop',
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
                y: json.temperatureTargetStart,
            });
            chart.data.datasets[2].data.push({
                x: json.time,
                y: json.temperatureTargetStop,
            });
            chart.data.datasets[3].data.push({
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

      stringReplace(html, "__TEMPERATURE_TARGET_START_LABEL__", "stop");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_LABEL__", "start");
    } else {
      stringReplace(html, "__TEMPERATURE_TARGET_START_COLOR__", "red");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_COLOR__", "green");

      stringReplace(html, "__TEMPERATURE_TARGET_START_LABEL__", "start");
      stringReplace(html, "__TEMPERATURE_TARGET_STOP_LABEL__", "stop");
    }

    stringReplace(
      html,
      "__TEMPERATURE_MIN__",
      std::to_string(static_cast<int>(round(temperatureMin / 10.0 - 1))).c_str()
    );
    stringReplace(
      html,
      "__TEMPERATURE_MAX__",
      std::to_string(static_cast<int>(round(temperatureMax / 10.0 + 1))).c_str()
    );

    stringReplace(html, "__HUMIDITY_MIN__", std::to_string(static_cast<int>(round(humidityMin / 10.0 - 1))).c_str());
    stringReplace(html, "__HUMIDITY_MAX__", std::to_string(static_cast<int>(round(humidityMax / 10.0 + 1))).c_str());

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
