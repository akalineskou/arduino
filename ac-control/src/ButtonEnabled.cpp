#include <Arduino.h>
#include "ButtonEnabled.h"
#include "Directive.h"

ButtonEnabled::ButtonEnabled(const int pin): pin(pin) {
  lastCall = 0;
#ifdef SIMULATE_BUTTON_ENABLE
  simulated = false;
#endif

  enabled = false;
  hasChanged = false;
}

void ButtonEnabled::setup() const {
  pinMode(pin, INPUT);
}

void ButtonEnabled::loop() {
  check();

  if (hasChanged) {
    // show temperature change for .0 and .5 temperatures
    Serial.printf("Enabled button change: %s -> %s.\n", enabled ? "Off" : "On", enabled ? "On" : "Off");
  }
}

void ButtonEnabled::check() {
  hasChanged = false;

#ifdef SIMULATE_BUTTON_ENABLE
  if (simulated) {
    return;
  }

  simulated = true;
#else
  if (digitalRead(pin) != HIGH) {
    return;
  }
#endif

  if (millis() - lastCall < 2 * 1000) {
    // change button value every 2 seconds
    return;
  }
  lastCall = millis();

  enabled = !enabled;

  hasChanged = true;
}
