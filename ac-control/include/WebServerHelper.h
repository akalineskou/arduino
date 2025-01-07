#pragma once

#include <WebServer.h>
#include "ACControl.h"
#include "ACMode.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TemperatureSensorManager.h"

class WebServerHelper {
  WebServer webServer;
  ACControl &acControl;
  TemperatureSensorManager &temperatureSensorManager;
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  ACMode acMode;

public:
  explicit WebServerHelper(
    ACControl &acControl,
    TemperatureSensorManager &temperatureSensorManager,
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData,
    const ACMode &acMode
  );

  void setup(const char *webServerAuthUsername, const char *webServerAuthPassword);

  void loop();
};
