#include <Arduino.h>

#include "TimeDelay.h"

TimeDelay::TimeDelay(const unsigned long delay) : delay(delay) {
  start();
}

bool TimeDelay::finished(const bool force, const bool restart) {
  if (running && (force || millis() - startTime >= delay)) {
    // don't stop and restart if forced finished
    if (!force) {
      running = false;

      if (restart) {
        start();
      }
    }

    return true;
  }

  return false;
}

void TimeDelay::start() {
  startTime = millis();
  running = true;
}
