#pragma once

#include <Arduino.h>

#include "Band.h"
#include "Beat.h"
#include "Directive.h"

struct Bands {
  int data[BLOCK_SIZE_HALF]{};

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
