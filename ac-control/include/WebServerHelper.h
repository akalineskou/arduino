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
  DatabaseHelper &databaseHelper;
  ACMode &acMode;

  WebServer webServer;

 public:
  explicit WebServerHelper(
    ACControl &acControl,
    TemperatureSensor &temperatureSensor,
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData,
    DatabaseHelper &databaseHelper,
    ACMode &acMode
  );

  void setup(const char* webServerAuthUsername, const char* webServerAuthPassword);

  void loop();

 private:
  bool isAuthenticated(const char* webServerAuthUsername, const char* webServerAuthPassword);

  static void stringReplace(std::string &string, const char* find, const char* replace);
};
