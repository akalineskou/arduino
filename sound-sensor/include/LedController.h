#pragma once

#include "Beat.h"
#include "Led.h"

class LedController {
  const Led dum;
  const Led tek;

 public:
  explicit LedController(int dumPin, int tekPin);

  void setup() const;

  void on(Beat beat) const;

  void off(Beat beat) const;

  static int offDelay(Beat beat);
};
