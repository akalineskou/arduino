#pragma once

#include <IRsend.h>
#include "ACMode.h"

class IRData {
public:
  uint8_t Off[kHaierAC160StateLength];
  uint8_t Start[kHaierAC160StateLength];
  uint8_t Stop[kHaierAC160StateLength];

  explicit IRData(ACMode acMode);
};
