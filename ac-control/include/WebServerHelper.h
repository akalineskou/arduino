#pragma once

#include <WebServer.h>
#include "WebServerTemplate.h"

class WebServerHelper {
  WebServer webServer;
  WebServerTemplate &webServerTemplate;

public:
  explicit WebServerHelper(WebServerTemplate &webServerTemplate);

  void setup();

  void loop();
};
