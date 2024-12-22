#include <Arduino.h>
#include <IRsend.h>
#include "IRData.h"

// @TODO: check ac led light is disabled in remote control
IRData::IRData(const ACMode acMode) {
  // off
  uint8_t off[kHaierAC160StateLength] = {
    0xA6, 0xE8, 0xB7, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x05, 0xEA, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5
  };
  memcpy(Off, off, sizeof(off));

  if (acMode == Cold) {
    // @todo
    // cold start -> 16°C
    uint8_t start[kHaierAC160StateLength] = {
      0xA6, 0xE8, 0xB6, 0x00, 0x7B, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5
    };
    memcpy(Start, start, sizeof(start));

    // @todo
    // cold stop -> 30°C
    uint8_t stop[kHaierAC160StateLength] = {
      0xA6, 0x08, 0xB7, 0x00, 0x45, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4B, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5
    };
    memcpy(Stop, stop, sizeof(stop));
  } else {
    // heat start -> 30°C
    uint8_t start[kHaierAC160StateLength] = {
      0xA6, 0xE8, 0xB6, 0x00, 0x7B, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5
    };
    memcpy(Start, start, sizeof(start));

    // heat stop -> 16°C
    uint8_t stop[kHaierAC160StateLength] = {
      0xA6, 0x08, 0xB7, 0x00, 0x45, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4B, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5
    };
    memcpy(Stop, stop, sizeof(stop));
  }
}
