#include <Arduino.h>
#include "ButtonEnabled.h"

void ButtonEnabled::setup() const {
  pinMode(pin, INPUT);
}

void ButtonEnabled::loop() {
  hasChanged = false;

  if (digitalRead(pin) != HIGH) {
    return;
  }

  if (millis() - lastCall < 2 * 1000) {
    // change button value every 2 seconds
    return;
  }
  lastCall = millis();

  enabled = !enabled;

  hasChanged = true;
}
