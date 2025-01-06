#include "WebServerHelper.h"

WebServerHelper::WebServerHelper(
  ButtonEnabled &buttonEnabled,
  TemperatureSensorManager &temperatureSensorManager,
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData,
  const ACMode &acMode
): webServer(80),
   buttonEnabled(buttonEnabled),
   temperatureSensorManager(temperatureSensorManager),
   infraredTransmitter(infraredTransmitter),
   temperatureData(temperatureData),
   acMode(acMode) {
}

void WebServerHelper::setup() {
  webServer.begin();

  webServer.on("/", HTTP_GET, [this] {
    auto html = String(R"==(
<html>
<head>
  <title>A/C Control</title>
  <meta charset="utf-8">
  <meta http-equiv="refresh" content="5">
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
    __ENABLED_TEMPLATE__
  </p>
  <p>
    Last A/C Command: <b>__LAST_AC_COMMAND__</b>
  </p>
  <p>
    Force A/C Command: <b><a href="/force-ac-start">Start</a></b> | <b><a href="/force-ac-stop">Stop</a></b>
  </p>
  <br>

  <p>
    <a href="/decrease-target">__TARGET_TEMPERATURE_DECREASE__</a>
    &nbsp; | &nbsp; Target: <b>__TEMPERATURE_TARGET__</b>
    &nbsp; | &nbsp; <a href="/increase-target">__TARGET_TEMPERATURE_INCREASE__</a>
  </p>
  <br>

  <p>
    Temperature: <b>__TEMPERATURE_IN__</b> <small>(__TEMPERATURE_OUT__)</small>
  </p>
  <p>
    __TEMPERATURE_START_STOP__
  </p>
  <br>

  <p>
    Humidity: <b>__HUMIDITY_IN__</b> <small>(__HUMIDITY_OUT__)</small>
  </p>
</body>
    )==");

    html.replace(
      "__ENABLED_TEMPLATE__",
      buttonEnabled.enabled ? R"==(<a href="/disable">Disable A/C Control</a>)==" : R"==(<a href="/enable">Enable A/C Control</a>)=="
    );
    html.replace("__TEMPERATURE_IN__", TemperatureSensor::formatTemperature(temperatureSensorManager.temperatureIn()).c_str());
    html.replace("__TEMPERATURE_OUT__", TemperatureSensor::formatTemperature(temperatureSensorManager.temperatureOut()).c_str());
    html.replace("__HUMIDITY_IN__", TemperatureSensor::formatHumidity(temperatureSensorManager.humidityIn()).c_str());
    html.replace("__HUMIDITY_OUT__", TemperatureSensor::formatHumidity(temperatureSensorManager.humidityOut()).c_str());
    html.replace("__LAST_AC_COMMAND__", ACCommands[infraredTransmitter.lastACCommand]);

    html.replace("__TEMPERATURE_TARGET__", TemperatureSensor::formatTemperature(temperatureData.temperatureTarget).c_str());
    if (acMode == Cold) {
      html.replace(
        "__TEMPERATURE_START_STOP__",
        "Stop <small>&lt;</small> <b>__TEMPERATURE_STOP__</b> &nbsp; | &nbsp; Start <small>&gt;</small> <b>__TEMPERATURE_START__</b>"
      );
    } else {
      html.replace(
        "__TEMPERATURE_START_STOP__",
        "Start <b><small>&lt;</small> __TEMPERATURE_START__</b> &nbsp; | &nbsp; Stop <b><small>&gt;</small> __TEMPERATURE_STOP__</b>"
      );
    }
    html.replace("__TEMPERATURE_START__", TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStart()).c_str());
    html.replace("__TEMPERATURE_STOP__", TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStop()).c_str());
    html.replace(
      "__TARGET_TEMPERATURE_INCREASE__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTarget + static_cast<int>(0.5 * 10)).c_str()
    );
    html.replace(
      "__TARGET_TEMPERATURE_DECREASE__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTarget - static_cast<int>(0.5 * 10)).c_str()
    );

    webServer.send(200, "text/html", html.c_str());
  });

  webServer.on("/enable", HTTP_GET, [this] {
    buttonEnabled.manualChange = true;

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/disable", HTTP_GET, [this] {
    buttonEnabled.manualChange = true;

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });

  webServer.on("/increase-target", HTTP_GET, [this] {
    temperatureData.temperatureTarget += 0.5 * 10;

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/decrease-target", HTTP_GET, [this] {
    temperatureData.temperatureTarget -= 0.5 * 10;

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/force-ac-start", HTTP_GET, [this] {
    infraredTransmitter.sendCommand(Start, true);

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/force-ac-stop", HTTP_GET, [this] {
    infraredTransmitter.sendCommand(Stop, true);

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });

  webServer.onNotFound([this] {
    webServer.send(404, "text/html", R"==(
<html>
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
