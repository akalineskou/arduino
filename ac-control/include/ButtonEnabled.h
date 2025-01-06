#pragma once

#include "millisDelay.h"

class ButtonEnabled {
  int pin;

  millisDelay timeDelay;

public:
  bool enabled;
  bool hasChanged;
  bool manualChange;

  explicit ButtonEnabled(int pin);

  void setup();

  void loop();

private:
  void check();
};
