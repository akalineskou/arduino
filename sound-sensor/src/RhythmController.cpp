#include <Arduino.h>

#include "Directive.h"
#include "RhythmController.h"

RhythmController::RhythmController(const LedController ledController): ledController(ledController) {
  globalBeatCount = 0;
  beatsCount = 0;
  currentBeat = 0;
  bpm = 0;
  previousMillis = 0;
  previousBeatValue = 0;
}

void RhythmController::play(const Rhythm rhythm, const int _bpm, int &beatCount) {
  bpm = _bpm;
  globalBeatCount = beatCount;

  currentBeat = 0;

  switch (rhythm) {
    case Bayo:
      beatsCount = 5;

      beat(Dum, 4);

      beat(Tek, 8);
      beat(Tek, 8);
      beat(Dum, 4);

      beat(Tek, 4);
      break;

    case Chiftetelli:
      beatsCount = 5;

      beat(Dum, 8);
      beat(Tek, 4);

      beat(Tek, 8);
      beat(Dum, 4);

      beat(Tek, 4);
      break;

    case Zonaradikos:
      beatsCount = 5;

      beat(Dum, 4);

      beat(Tek, 8);
      beat(Tek, 8);
      beat(Tek, 8);
      beat(Tek, 8);
      break;

    case Tsamikos:
      beatsCount = 8;

      beat(Dum, 8 + 8 / 2);

      beat(Tek, 16);
      beat(Tek, 8);
      beat(Tek, 8);

      beat(Dum, 16);
      beat(Tek, 16);
      beat(Tek, 16);
      beat(Tek, 16);
      break;
  }

  beatCount = globalBeatCount;
}

void RhythmController::beat(const Beat beat, const int beatValue) {
  currentBeat++;
  if (currentBeat != globalBeatCount) {
    return;
  }

  const unsigned long currentMillis = millis();

  if (previousBeatValue > 0 &&
      currentMillis - previousMillis < static_cast<int>(round(4.0 / previousBeatValue * 60.0 / bpm * 1000))) {
    return;
  }

  previousMillis = currentMillis;
  previousBeatValue = beatValue;

  ledController.on(beat);
  delay(50);
  ledController.off(beat);

  if (globalBeatCount == beatsCount) {
    globalBeatCount = 1;

#if DEBUG
    Serial.println();
#endif
    return;
  }

  globalBeatCount++;
}
