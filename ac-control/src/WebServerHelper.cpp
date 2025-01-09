#include "Directive.h"
#include "WebServerHelper.h"

WebServerHelper::WebServerHelper(
  ACControl &acControl,
  TemperatureSensor &temperatureSensor,
  InfraredTransmitter &infraredTransmitter,
  TemperatureData &temperatureData,
  const ACMode &acMode
):
    acControl(acControl),
    temperatureSensor(temperatureSensor),
    infraredTransmitter(infraredTransmitter),
    temperatureData(temperatureData),
    acMode(acMode),
    webServer(80),
    timeDelay(TimeDelay(1 * 1000, true)) {}

void WebServerHelper::setup(const char* webServerAuthUsername, const char* webServerAuthPassword) {
  webServer.begin();

  webServer.on("/", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if DEBUG
    Serial.println("GET /");
#endif

    auto html = String(R"==(
<html>
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
</body>
    )==");

    html.replace(
      "__AC_CONTROL_STATUS__",
      acControl.enabled ? R"==(<b>Enabled</b> (<a href="/disable">Disable</a>))=="
                        : R"==(<b>Disabled</b> (<a href="/enable">Enable</a>))=="
    );
    html.replace("__LAST_AC_COMMAND__", ACCommands[infraredTransmitter.lastACCommand]);

    html.replace("__TEMPERATURE__", TemperatureSensor::formatTemperature(temperatureSensor.getTemperature()).c_str());
    html.replace("__HUMIDITY__", TemperatureSensor::formatHumidity(temperatureSensor.getHumidity()).c_str());

    html.replace(
      "__TEMPERATURE_TARGET__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTarget).c_str()
    );
    if (acMode == Cold) {
      html.replace(
        "__TEMPERATURE_START_STOP__",
        "Stop <small>&lt;</small> <b>__TEMPERATURE_STOP__</b> "
        "&nbsp; | "
        "&nbsp; Start <small>&gt;</small> "
        "<b>__TEMPERATURE_START__</b>"
      );
    } else {
      html.replace(
        "__TEMPERATURE_START_STOP__",
        "Start <b><small>&lt;</small> __TEMPERATURE_START__</b> "
        "&nbsp; | "
        "&nbsp; Stop <b><small>&gt;</small> "
        "__TEMPERATURE_STOP__</b>"
      );
    }
    html.replace(
      "__TEMPERATURE_START__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStart()).c_str()
    );
    html.replace(
      "__TEMPERATURE_STOP__",
      TemperatureSensor::formatTemperature(temperatureData.temperatureTargetStop()).c_str()
    );
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

  webServer.on("/enable", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if DEBUG
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

#if DEBUG
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

#if DEBUG
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

#if DEBUG
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

#if DEBUG
    Serial.println("GET /force-ac-start");
#endif

    infraredTransmitter.sendCommand(Start, true);

    webServer.sendHeader("Location", "/", true);
    webServer.send(302);
  });
  webServer.on("/force-ac-stop", HTTP_GET, [this, webServerAuthUsername, webServerAuthPassword] {
    if (!isAuthenticated(webServerAuthUsername, webServerAuthPassword)) {
      return webServer.requestAuthentication();
    }

#if DEBUG
    Serial.println("GET /force-ac-stop");
#endif

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
  if (!timeDelay.delayPassed()) {
    return;
  }

  webServer.handleClient();
}

bool WebServerHelper::isAuthenticated(const char* webServerAuthUsername, const char* webServerAuthPassword) {
  if (strlen(webServerAuthUsername) == 0 && strlen(webServerAuthPassword) == 0) {
    // authentication disabled
    return true;
  }

  return webServer.authenticate(webServerAuthUsername, webServerAuthPassword);
}
