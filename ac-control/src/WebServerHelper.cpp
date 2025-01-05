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
  <meta http-equiv="refresh" content="10">
  <link rel="shortcut icon" href="https://cdn-icons-png.flaticon.com/512/3274/3274588.png" />
</head>
<body style="text-align: center;">
  <p>__ENABLED_TEMPLATE__</p>
  <p>Temperature in: __TEMPERATURE_IN__.</p>
  <p>Temperature Out: __TEMPERATURE_OUT__.</p>
  <p>Last A/C Command: __LAST_AC_COMMAND__.</p>
  <br>

  <p>Target temperature: __TEMPERATURE_TARGET__.</p>
  <p>__TEMPERATURE_START_STOP__</p>
  <br>

  <p><a href="/increase-target">Increase target temperature by 0.5°C.</a></p>
  <p><a href="/decrease-target">Decrease target temperature by 0.5°C.</a></p>
</body>
    )==");

    html.replace(
      "__ENABLED_TEMPLATE__",
      String(buttonEnabled.enabled
               ? R"==(<a href="/disable">Disable A/C control.</a>)=="
               : R"==(<a href="/enable">Enable A/C control.</a>)==").c_str()
    );
    html.replace("__TEMPERATURE_IN__", TemperatureSensor::formatTemperature(temperatureSensorManager.temperatureIn()).c_str());
    html.replace("__TEMPERATURE_OUT__", TemperatureSensor::formatTemperature(temperatureSensorManager.temperatureOut()).c_str());
    html.replace("__LAST_AC_COMMAND__", ACCommands[infraredTransmitter.lastACCommand]);

    html.replace("__TEMPERATURE_TARGET__", TemperatureSensor::formatTemperature(temperatureData.temperatureTarget).c_str());
    if (acMode == Cold) {
      html.replace("__TEMPERATURE_START_STOP__", "Stop when < __TEMPERATURE_STOP__ | Start when > __TEMPERATURE_START__");
    } else {
      html.replace("__TEMPERATURE_START_STOP__", "Start when < __TEMPERATURE_START__ | Stop when > __TEMPERATURE_STOP__");
    }
    html.replace("__TEMPERATURE_START__", TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStart()).c_str());
    html.replace("__TEMPERATURE_STOP__", TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStop()).c_str());

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

  webServer.onNotFound([this] {
    webServer.send(404, "text/html", String(R"==(
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
    )==").c_str());
  });
}

void WebServerHelper::loop() {
  webServer.handleClient();
}
