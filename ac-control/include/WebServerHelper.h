#pragma once

#include <WebServer.h>

#include "ACControl.h"
#include "ACMode.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"

class WebServerHelper {
  ACControl &acControl;
  TemperatureSensor &temperatureSensor;
  TemperatureData &temperatureData;
  TimeHelper &timeHelper;
  DatabaseHelper &databaseHelper;
  ACMode &acMode;

  WebServer webServer;

 public:
  explicit WebServerHelper(
    ACControl &acControl,
    TemperatureSensor &temperatureSensor,
    TemperatureData &temperatureData,
    TimeHelper &timeHelper,
    DatabaseHelper &databaseHelper,
    ACMode &acMode
  );

  void setup();

  void loop();
};
