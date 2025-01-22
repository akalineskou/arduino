#include "Bands.h"

Band::Band(const int count, int bands[], const int minValue): count(count), bands(bands), minValue(minValue) {}

Bands::Bands():
    dumCount(1),
    dumBands(new Band[1]{Band(1, new int[1]{3}, 55000)}),
    tekCount(1),
    tekBands(new Band[1]{
      Band(9, new int[9]{15, 16, 17, 18, 21, 22, 23, 24, 25}, 5000),
    }) {}

int Bands::getBeat(const bool idling) const {
  if (idling) {
    return data[1] > 100 ? Dum : -1;
  }

  int maxDum = 0;

  for (auto i = 0; i < dumCount; i++) {
    const auto band = dumBands[i];

    for (auto j = 0; j < band.count; j++) {
      if (data[band.bands[j]] > band.minValue) {
        return Dum;
      }

      if (data[band.bands[j]] > maxDum) {
        maxDum = data[band.bands[j]];
      }
    }
  }

  for (auto i = 0; i < tekCount; i++) {
    const auto band = tekBands[i];

    for (auto j = 0; j < band.count; j++) {
      if (data[band.bands[j]] > band.minValue * (maxDum > 20000 ? 2 : 1)) {
        return Tek;
      }
    }
  }

  return -1;
}

#if APP_CHART
std::string Bands::chartJson() const {
  std::string json = "[";

  for (auto i = 0; i < BAND_SIZE; i++) {
  #if !APP_CHART_INCLUDE_ALL
    bool allowed = false;

    // use lambda to break from 2nd for loop
    [&] {
      for (auto j = 0; j < dumCount; j++) {
        for (auto k = 0; k < dumBands[j].count; k++) {
          if (i == dumBands[j].bands[k]) {
            allowed = true;

            return;
          }
        }
      }

      for (auto j = 0; j < tekCount; j++) {
        for (auto k = 0; k < tekBands[j].count; k++) {
          if (i == tekBands[j].bands[k]) {
            allowed = true;

            return;
          }
        }
      }
    }();

    if (!allowed) {
      continue;
    }
  #endif

    json += "{\"label\":" + std::to_string(i) + ", \"value\": " + std::to_string(data[i]) + "},";
  }

  // Remove trailing comma
  json.pop_back();

  json += "]";

  return json;
}
#endif
