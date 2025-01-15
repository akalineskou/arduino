#pragma once

enum Beat {
  Dum,
  Tek,
};

static const char* Beats[] = {
  "Dum",
  "Tek",
};

struct BeatDatum {
  Beat beat;
  int value;

  BeatDatum(const Beat beat, const int value): beat(beat), value(value) {}
};

struct BeatData {
  int count;
  BeatDatum* beatDatum;

  explicit BeatData(const int count, BeatDatum* beatDatum): count(count), beatDatum(beatDatum) {}
};
