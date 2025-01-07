#include <Arduino.h>
#include "TimeDelay.h"

TimeDelay::TimeDelay(const unsigned long delay): delay(delay) {
  startTime = millis();
}

bool TimeDelay::finished(const bool finishNow, const bool restart) {
  if (finishNow || millis() - startTime >= delay) {
    if (restart && !finishNow) {
      startTime = millis();
    }

    return true;
  }

  return false;
}
