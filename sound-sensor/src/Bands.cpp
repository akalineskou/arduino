#include "Bands.h"

Bands::Bands():
    dumBands(new Band[1]{Band(new int[4]{11, 12, 13, 14}, 4, 110000)}),
    dumCount(1),
    tekBands(new Band[4]{
      Band(new int[4]{78, 79, 80, 81}, 4, 15000),
      Band(new int[4]{87, 88, 89, 90}, 4, 15000),
      Band(new int[5]{100, 101, 102, 103, 104}, 5, 10000),
      Band(new int[5]{117, 118, 119, 120, 121}, 5, 10000),
    }),
    tekCount(4) {}

int Bands::value(const Band &band) const {
  int value = 0;

  for (auto i = 0; i < band.count; i++) {
    value += data[band.bands[i]];
  }

  return value / band.count;
}

bool Bands::is(const Beat beat) const {
  for (auto i = 0; i < (beat == Dum ? dumCount : tekCount); i++) {
    const auto band = (beat == Dum ? dumBands : tekBands)[i];

    if (value(band) > band.minValue) {
      return true;
    }
  }

  return false;
}

#if CHART
std::string Bands::chartJson() const {
  std::string json = "[";

  for (auto i = 0; i < dumCount; i++) {
    json += R"({"label":"Dum", "value": )" + std::to_string(value(dumBands[i])) + "},";
  }
  for (auto i = 0; i < tekCount; i++) {
    json += R"({"label":"Tek", "value": )" + std::to_string(value(tekBands[i])) + "},";
  }

  for (auto i = 0; i < BLOCK_SIZE_HALF; i++) {
  #if !CHART_INCLUDE_ALL
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
