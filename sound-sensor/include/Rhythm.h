#pragma once

#include "Beat.h"

struct RhythmData {
  const char* name;
  const int bpm;
  const BeatData* beatData;

  RhythmData(const char* name, const int bpm, const BeatData* beatData): name(name), bpm(bpm), beatData(beatData) {}
};

static const RhythmData* RhythmDatas[] = {
  new RhythmData(
    "Bayo 1",
    120,
    new BeatData(
      5,
      new BeatDatum[5]{
        BeatDatum(Dum, 4),

        BeatDatum(Tek, 8),
        BeatDatum(Tek, 8),
        BeatDatum(Dum, 4),

        BeatDatum(Tek, 4),
      }
    )
  ),

  new RhythmData(
    "Bayo 2",
    120,
    new BeatData(
      5,
      new BeatDatum[5]{
        BeatDatum(Dum, 8),

        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
        BeatDatum(Dum, 8),

        BeatDatum(Tek, 8),
      }
    )
  ),

  new RhythmData(
    "Chiftetelli 1",
    120,
    new BeatData(
      5,
      new BeatDatum[5]{
        BeatDatum(Dum, 8),
        BeatDatum(Tek, 4),

        BeatDatum(Tek, 8),
        BeatDatum(Dum, 4),

        BeatDatum(Tek, 4),
      }
    )
  ),

  new RhythmData(
    "Chiftetelli 2",
    120,
    new BeatData(
      11,
      new BeatDatum[11]{
        BeatDatum(Dum, 8),

        BeatDatum(Tek, 8),

        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
        BeatDatum(Tek, 8),

        BeatDatum(Dum, 8),

        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),

        BeatDatum(Tek, 8),

        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
      }
    )
  ),

  // Zonaradikos
  new RhythmData(
    "Zonaradikos",
    110,
    new BeatData(
      5,
      new BeatDatum[5]{
        BeatDatum(Dum, 8),

        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
      }
    )
  ),

  // Tsamikos
  new RhythmData(
    "Tsamikos",
    120,
    new BeatData(
      7,
      new BeatDatum[7]{
        BeatDatum(Dum, 8),

        BeatDatum(Tek, 16),
        BeatDatum(Tek, 16),
        BeatDatum(Tek, 8),

        BeatDatum(Tek, 8),

        BeatDatum(Dum, 8),

        BeatDatum(Tek, 8),
      }
    )
  ),

  //
  // new RhythmData(
  //   "",
  //   120,
  //   new BeatData(
  //     5,
  //     new BeatDatum[5]{
  //     }
  //   )
  // ),
};
