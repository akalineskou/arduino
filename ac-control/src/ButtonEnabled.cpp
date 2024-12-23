#include <Arduino.h>
#include "ButtonEnabled.h"

ButtonEnabled::ButtonEnabled(const int pin): pin(pin) {
  lastCall = 0;

  enabled = false;
  hasChanged = false;
  manualChange = false;
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

  if (!manualChange) {
    if (digitalRead(pin) != HIGH) {
      return;
    }

    if (millis() - lastCall < 2 * 1000) {
      // change button value every 2 seconds
      return;
    }
    lastCall = millis();
  } else {
    manualChange = false;
  }

  enabled = !enabled;
  hasChanged = true;
}
