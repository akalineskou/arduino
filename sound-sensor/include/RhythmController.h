#pragma once

#include "LedController.h"

enum Rhythm {
  Bayo,
  Chiftetelli,
  Zonaradikos,
  Tsamikos,
};

static const char* Rhythms[] = {
  "Bayo",
  "Chiftetelli",
  "Zonaradikos",
  "Tsamikos",
};

class RhythmController {
  LedController ledController;
  int globalBeatCount;
  int beatsCount;
  int currentBeat;
  int bpm;
  unsigned long previousMillis;
  unsigned long previousBeatValue;

 public:
  explicit RhythmController(LedController ledController);

  void play(Rhythm rhythm, int _bpm, int &beatCount);

 private:
  void beat(Beat beat, int beatValue);
};
