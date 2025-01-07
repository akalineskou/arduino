#include <Arduino.h>
#include "ButtonEnabled.h"

ButtonEnabled::ButtonEnabled(
  const int pin
): pin(pin),
   timeDelay(millisDelay()) {
  enabled = false;
  hasChanged = false;
  manualChange = false;
}

void ButtonEnabled::setup() {
  pinMode(pin, INPUT);

  // change button value every 2s
  timeDelay.start(2 * 1000);
}

void ButtonEnabled::loop() {
  check();

  if (hasChanged) {
    // show temperature change for .0 and .5 temperatures
#if DEBUG
    Serial.printf("Enabled button change: %s -> %s.\n", enabled ? "Off" : "On", enabled ? "On" : "Off");
#endif
  }
}

void ButtonEnabled::check() {
  hasChanged = false;

  if (!manualChange) {
    if (digitalRead(pin) != HIGH) {
      return;
    }

    if (!timeDelay.justFinished()) {
      return;
    }

    timeDelay.repeat();
  } else {
    manualChange = false;
  }

  enabled = !enabled;
  hasChanged = true;
}
