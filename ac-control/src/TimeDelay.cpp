#include <Arduino.h>

#include "TimeDelay.h"

TimeDelay::TimeDelay(const unsigned long delay, const bool repeat):
    delay(delay),
    repeat(repeat),
    forceFirstRun(repeat) {
  start();
}

bool TimeDelay::delayPassed(const bool force) {
  if (!running) {
    return false;
  }

  if (force) {
    return true;
  }

  bool delayPassed = false;

  if (forceFirstRun) {
    forceFirstRun = false;

    delayPassed = true;
  }

  if (millis() - startTime >= delay) {
    delayPassed = true;
  }

  if (delayPassed) {
    running = false;

    if (repeat) {
      start();
    }

    return true;
  }

  return false;
}

void TimeDelay::start() {
  startTime = millis();
  running = true;
}
