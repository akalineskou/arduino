#include <Arduino.h>
#include "ButtonEnabled.h"

void ButtonEnabled::setup() const {
  pinMode(pin, INPUT);
}

void ButtonEnabled::loop() {
  check();
}

void ButtonEnabled::check() {
  hasChanged = false;

  if (digitalRead(pin) != HIGH) {
    return;
  }

  if (millis() - lastCall < 2 * 1000) {
    // get button readings every 2 seconds
    return;
  }
  lastCall = millis();

  enabled = !enabled;

  hasChanged = true;
}
