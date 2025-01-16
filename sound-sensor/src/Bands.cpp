#include "Bands.h"

Band::Band(const int count, int bands[], const int minValue): count(count), bands(bands), minValue(minValue) {}

Bands::Bands():
    dumCount(1),
    dumBands(new Band[1]{Band(1, new int[1]{3}, 40000)}),
    tekCount(3),
    tekBands(new Band[3]{
      Band(2, new int[2]{17, 18}, 4000),
      Band(2, new int[2]{21, 22}, 4000),
      Band(2, new int[2]{23, 24}, 4000),
    }) {}

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

#if APP_CHART
std::string Bands::chartJson() const {
  std::string json = "[";

  for (auto i = 0; i < dumCount; i++) {
    json += R"({"label":"Dum", "value": )" + std::to_string(value(dumBands[i])) + "},";
  }
  for (auto i = 0; i < tekCount; i++) {
    json += R"({"label":"Tek", "value": )" + std::to_string(value(tekBands[i])) + "},";
  }

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
