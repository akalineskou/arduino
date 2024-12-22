#pragma once

#include "Directive.h"

class ButtonEnabled {
  int pin;

  unsigned long lastCall;
#ifdef SIMULATE_BUTTON_ENABLE
  bool simulated;
#endif

public:
  bool enabled;
  bool hasChanged;

  explicit ButtonEnabled(int pin);

  void setup() const;

  void loop();

private:
  void check();
};
