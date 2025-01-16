#include <Arduino.h>

#include "Directive.h"
#include "RhythmController.h"

RhythmController::RhythmController(const LedController ledController): ledController(ledController) {
  previousBeat = Dum;
  previousBeatValue = 0;
  previousBeatMillis = 0;
  ledOffMillis = 0;
}

void RhythmController::play(const RhythmData* rhythmData, int &beatCount) {
  unsigned long currentMillis = millis();
  if (currentMillis - ledOffMillis >= LedController::offDelay(previousBeat)) {
    ledController.off(previousBeat);

    ledOffMillis = currentMillis;
  }

  const auto beatData = rhythmData->beatData;
  for (auto i = 0; i < beatData->count; i++) {
    if (i + 1 != beatCount) {
      // not current beat
      continue;
    }

    currentMillis = millis();
    if (previousBeatValue > 0 &&
        currentMillis - previousBeatMillis <
          static_cast<int>(round(4.0 / previousBeatValue * 60.0 / rhythmData->bpm * 1000))) {
      // time has not passed
      continue;
    }

    previousBeat = beatData->beatDatum[i].beat;
    previousBeatValue = beatData->beatDatum[i].value;
    previousBeatMillis = currentMillis;

    ledController.on(beatData->beatDatum[i].beat);

    ledOffMillis = millis();

    if (beatCount == beatData->count) {
      // last beat
      beatCount = 1;

#if APP_DEBUG
      Serial.println();
#endif

      continue;
    }

    beatCount++;
  }
}
