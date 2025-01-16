#pragma once

#include <Arduino.h>

#include "Beat.h"
#include "Directive.h"

#define BAND_SIZE 32

struct Band {
  int count;
  int* bands;
  int minValue;

  Band(int count, int* bands, int minValue);
};

struct Bands {
  int data[BAND_SIZE]{};

  int dumCount;
  Band* dumBands;
  int tekCount;
  Band* tekBands;

  Bands();

  int getBeat(bool idling) const;

#if APP_CHART
  std::string chartJson() const;
#endif
};
