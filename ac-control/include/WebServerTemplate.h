#pragma once

#include <Arduino.h>
#include "TemperatureSensor.h"

class WebServerTemplate {
  TemperatureSensor &temperatureSensor;

public:
  explicit WebServerTemplate(
    TemperatureSensor &temperatureSensor
  );

  String index();

  String notFound();
};
