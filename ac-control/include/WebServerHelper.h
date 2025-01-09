#pragma once

#include <WebServer.h>

#include "ACControl.h"
#include "ACMode.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"

class WebServerHelper {
  ACControl &acControl;
  TemperatureSensor &temperatureSensor;
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  ACMode acMode;

  WebServer webServer;
  TimeDelay timeDelay;

 public:
  explicit WebServerHelper(
    ACControl &acControl,
    TemperatureSensor &temperatureSensor,
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData,
    const ACMode &acMode
  );

  void setup(const char* webServerAuthUsername, const char* webServerAuthPassword);

  void loop();

 private:
  bool isAuthenticated(const char* webServerAuthUsername, const char* webServerAuthPassword);
};
