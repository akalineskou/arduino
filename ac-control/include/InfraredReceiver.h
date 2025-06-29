#pragma once

#include <IRrecv.h>

#include "WebServerHelper.h"

class InfraredReceiver {
  int pin;
  ACControl &acControl;
  TemperatureData &temperatureData;

  IRrecv irRecv;

 public:
  explicit InfraredReceiver(int pin, ACControl &acControl, TemperatureData &temperatureData);

  void setup();

  void loop();
};
