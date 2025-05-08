#pragma once

#include <Arduino.h>

enum ACMode {
  Cold,
  Heat,
};

static const char* ACModes[] = {
  "Cold",
  "Heat",
};
static ACMode ACModesOther[] = {
  Heat,
  Cold,
};

inline ACMode sToACMode(const char* aCMode) {
  if (strcasecmp(aCMode, "Cold") == 0) {
    return Cold;
  }
  return Heat;
}
