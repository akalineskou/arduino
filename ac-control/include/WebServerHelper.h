#pragma once

#include <WebServer.h>
#include "ACMode.h"
#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TemperatureSensorManager.h"

class WebServerHelper {
  WebServer webServer;
  ButtonEnabled &buttonEnabled;
  TemperatureSensorManager &temperatureSensorManager;
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  ACMode acMode;

public:
  explicit WebServerHelper(
    ButtonEnabled &buttonEnabled,
    TemperatureSensorManager &temperatureSensorManager,
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData,
    const ACMode &acMode
  );

  void setup(const char *webServerAuthUsername, const char *webServerAuthPassword);

  void loop();
};
