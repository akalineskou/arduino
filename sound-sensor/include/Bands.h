#pragma once

#include <Arduino.h>

#include "Beat.h"
#include "Directive.h"

struct Band {
  int* bands;
  int count;
  int minValue;

  Band(int* bands, int count, int minValue);
};

struct Bands {
  int data[SAMPLES_HALF]{};

  Band* dumBands;
  int dumCount;
  Band* tekBands;
  int tekCount;

  Bands();

  int value(const Band &band) const;

  bool is(Beat beat) const;

#if CHART
  std::string chartJson() const;
#endif
};
