#pragma once

#include <WebServer.h>
#include "ACMode.h"
#include "ButtonEnabled.h"
#include "InfraredTransmitter.h"
#include "TemperatureData.h"
#include "TemperatureSensor.h"

class WebServerHelper {
  WebServer webServer;
  ButtonEnabled &buttonEnabled;
  TemperatureSensor &temperatureSensor;
  InfraredTransmitter &infraredTransmitter;
  TemperatureData &temperatureData;
  ACMode acMode;

public:
  explicit WebServerHelper(
    ButtonEnabled &buttonEnabled,
    TemperatureSensor &temperatureSensor,
    InfraredTransmitter &infraredTransmitter,
    TemperatureData &temperatureData,
    const ACMode &acMode
  );

  void setup();

  void loop();
};
