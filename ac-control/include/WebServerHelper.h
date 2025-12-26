#pragma once

#include <WebServer.h>

#include "ACControl.h"
#include "TemperatureData.h"

class WebServerHelper {
  ACControl &acControl;
  TimeHelper &timeHelper;
  DatabaseHelper &databaseHelper;

  WebServer webServer;

 public:
  explicit WebServerHelper(ACControl &acControl, TimeHelper &timeHelper, DatabaseHelper &databaseHelper);

  void setup();

  void loop();
};
