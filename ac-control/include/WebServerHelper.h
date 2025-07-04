#pragma once

#include <WebServer.h>

#include "ACControl.h"
#include "TemperatureData.h"

class WebServerHelper {
  ACControl &acControl;
  TimeHelper &timeHelper;

  WebServer webServer;

 public:
  explicit WebServerHelper(ACControl &acControl, TimeHelper &timeHelper);

  void setup();

  void loop();
};
