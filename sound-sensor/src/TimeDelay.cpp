#include <Arduino.h>

#include "TimeDelay.h"

TimeDelay::TimeDelay(const unsigned long delay): TimeDelay(delay, false) {
  start();
}

TimeDelay::TimeDelay(const unsigned long delay, const bool repeat):
    delay(delay),
    repeat(repeat),
    forceFirstRun(repeat) {
  start();
}

bool TimeDelay::delayPassed(const bool force) {
  if (finished) {
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
    finished = true;

    if (repeat) {
      start();
    }

    return true;
  }

  return false;
}

void TimeDelay::restart() {
  start();
}

void TimeDelay::start() {
  startTime = millis();
  finished = false;
}
