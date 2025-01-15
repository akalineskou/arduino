#pragma once

#include "LedController.h"
#include "Rhythm.h"

class RhythmController {
  LedController ledController;
  Beat previousBeat;
  unsigned long previousBeatValue;
  unsigned long previousBeatMillis;
  unsigned long ledOffMillis;

 public:
  explicit RhythmController(LedController ledController);

  void play(const RhythmData* rhythmData, int &beatCount);
};
